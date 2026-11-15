#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

namespace spatiallab
{
struct Stereo { float left = 0.0f, right = 0.0f; };
struct Reflections { Stereo early, late; };

// A finite-duration linear ramp. Targets may be refreshed each host block.
class Ramp
{
public:
    void reset(double rate, float value) noexcept
    { length = std::max(1, static_cast<int>(rate * 0.05)); current = target = value; remaining = 0; }
    void set(float value) noexcept
    { if (value != target) { target = value; remaining = length; step = (target - current) / static_cast<float>(length); } }
    float next() noexcept
    { if (remaining > 0 && --remaining == 0) current = target; else if (remaining > 0) current += step; return current; }
private:
    float current = 0, target = 0, step = 0;
    int remaining = 0, length = 1;
};

struct RoomSpec
{
    float rt60, cutoff;
    std::array<float, 8> delaysMs;
    std::array<float, 6> earlyMs;
    float earlyLevel = 1, lateLevel = 1, earlyCrossfeed = 0.2f, diffusionAngle = 0;
    std::array<float, 6> reflectionGains {0.30f, -0.23f, 0.19f, 0.15f, -0.12f, 0.09f};
};
// Phase 2 proposal: compact residential speech rooms, neutral Size/Tone = 0.5.
inline constexpr std::array<RoomSpec, 3> rooms {{
    {0.40f, 3400.0f, {23.3f, 29.9f, 34.7f, 39.1f, 43.7f, 48.7f, 53.3f, 59.9f},
     {5.3f, 9.1f, 14.7f, 20.9f, 28.3f, 36.7f}, 1.30f, 1.15f, 0.30f, 0.48f,
     {0.30f, -0.25f, 0.22f, 0.18f, -0.14f, 0.10f}},
    {0.32f, 5200.0f, {13.7f, 17.9f, 21.1f, 25.3f, 29.9f, 33.7f, 37.1f, 41.9f},
     {2.3f, 4.7f, 7.1f, 10.9f, 14.3f, 19.1f}, 1.08f, 0.95f, 0.22f, 0.62f,
     {0.32f, -0.24f, 0.21f, -0.16f, 0.12f, 0.09f}},
    {0.38f, 7600.0f, {9.7f, 12.1f, 14.9f, 17.3f, 20.9f, 23.9f, 27.7f, 31.1f},
     {1.4f, 2.9f, 4.3f, 6.7f, 9.1f, 13.1f}, 1.12f, 0.95f, 0.14f, 0.28f,
     {0.34f, -0.27f, 0.23f, -0.18f, 0.14f, -0.10f}}
}};

class Room
{
public:
    void prepare(double rate, const RoomSpec& spec, float decay, float tone)
    {
        sampleRate = rate;
        earlyScale = spec.earlyLevel; lateScale = spec.lateLevel; crossfeed = spec.earlyCrossfeed;
        reflectionGains = spec.reflectionGains;
        scatterCos = std::cos(spec.diffusionAngle); scatterSin = std::sin(spec.diffusionAngle);
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            lines[i].assign(static_cast<std::size_t>(std::max(1.0, std::round(rate * spec.delaysMs[i] * 0.001))), 0.0f);
            positions[i] = 0;
            delaySeconds[i] = static_cast<float>(lines[i].size() / rate);
            feedback[i].reset(rate, gain(spec.rt60 * decay, delaySeconds[i]));
        }
        earlyL.assign(static_cast<std::size_t>(rate * 0.05) + 2, 0.0f);
        earlyR = earlyL;
        for (std::size_t i = 0; i < tapsL.size(); ++i)
        {
            tapsL[i] = static_cast<std::size_t>(std::round(rate * spec.earlyMs[i] * 0.001));
            tapsR[i] = static_cast<std::size_t>(std::round(rate * (spec.earlyMs[i] * 1.071f + 0.37f) * 0.001));
        }
        pole.reset(rate, coefficient(spec.cutoff * (std::exp2(2.0f * (tone - 0.5f)))));
        lastDecay = decay; lastTone = tone;
        filtered.fill(0.0f); earlyPos = 0;
    }
    void set(float decay, float tone, const RoomSpec& spec, bool immediate = false) noexcept
    {
        if (immediate || decay != lastDecay)
        {
            for (std::size_t i = 0; i < feedback.size(); ++i)
                if (immediate) feedback[i].reset(sampleRate, gain(spec.rt60 * decay, delaySeconds[i]));
                else feedback[i].set(gain(spec.rt60 * decay, delaySeconds[i]));
            lastDecay = decay;
        }
        if (immediate || tone != lastTone)
        {
            const float value = coefficient(spec.cutoff * std::exp2(2.0f * (tone - 0.5f)));
            if (immediate) pole.reset(sampleRate, value); else pole.set(value);
            lastTone = tone;
        }
    }
    void reset() noexcept
    {
        for (auto& line : lines) std::fill(line.begin(), line.end(), 0.0f);
        std::fill(earlyL.begin(), earlyL.end(), 0.0f);
        std::fill(earlyR.begin(), earlyR.end(), 0.0f);
        positions.fill(0); filtered.fill(0); earlyPos = 0;
    }
    Reflections process(float left, float right) noexcept
    {
        earlyL[earlyPos] = left; earlyR[earlyPos] = right;
        Stereo early;
        const auto& tapGain = reflectionGains;
        for (std::size_t i = 0; i < tapGain.size(); ++i)
        {
            const auto l = earlyPos >= tapsL[i] ? earlyPos - tapsL[i] : earlyPos + earlyL.size() - tapsL[i];
            const auto r = earlyPos >= tapsR[i] ? earlyPos - tapsR[i] : earlyPos + earlyR.size() - tapsR[i];
            early.left += tapGain[i] * ((1.0f - crossfeed) * earlyL[l] + crossfeed * earlyR[l]);
            early.right += tapGain[i] * ((1.0f - crossfeed) * earlyR[r] + crossfeed * earlyL[r]);
        }
        if (++earlyPos == earlyL.size()) earlyPos = 0;
        std::array<float, 8> y {};
        float sum = 0;
        const float p = pole.next();
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            y[i] = lines[i][positions[i]];
            filtered[i] = (1.0f - p) * y[i] + p * filtered[i];
            sum += filtered[i];
        }
        // Orthogonal Householder scattering; loop gains < 1 guarantee passive decay.
        constexpr std::array<float, 8> injectionL {1, 1, -1, -1, 1, -1, 1, -1};
        constexpr std::array<float, 8> injectionR {1, -1, 1, -1, -1, 1, 1, -1};
        // Pair rotations followed by Householder scattering remain orthogonal:
        // room-specific diffusion changes density without injecting loop energy.
        std::array<float, 8> scattered {};
        for (std::size_t i = 0; i < 8; i += 2)
        {
            const float a = filtered[i] - 0.25f * sum, b = filtered[i + 1] - 0.25f * sum;
            scattered[i] = scatterCos * a + scatterSin * b;
            scattered[i + 1] = -scatterSin * a + scatterCos * b;
        }
        Stereo late;
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            lines[i][positions[i]] = 0.20f * (injectionL[i] * left + injectionR[i] * right)
                + feedback[i].next() * scattered[i];
            if (++positions[i] == lines[i].size()) positions[i] = 0;
            // Different readout vectors avoid a direct comb-like stereo image.
            late.left += y[i] * injectionR[i] * 0.25f;
            late.right += y[i] * injectionL[(i + 3) % 8] * 0.25f;
        }
        return {{early.left * earlyScale, early.right * earlyScale},
                {late.left * lateScale, late.right * lateScale}};
    }
private:
    static float gain(float rt60, float delay) noexcept { return std::pow(10.0f, -3.0f * delay / rt60); }
    float coefficient(float hz) const noexcept
    { return std::exp(-6.28318530718f * std::min(hz, static_cast<float>(sampleRate * 0.45)) / static_cast<float>(sampleRate)); }
    float earlyScale = 1, lateScale = 1, crossfeed = 0.2f, scatterCos = 1, scatterSin = 0;
    std::array<float, 6> reflectionGains {};
    double sampleRate = 48000;
    float lastDecay = 1, lastTone = 0.5f;
    std::array<std::vector<float>, 8> lines;
    std::vector<float> earlyL, earlyR;
    std::array<std::size_t, 8> positions {};
    std::array<std::size_t, 6> tapsL {}, tapsR {};
    std::array<float, 8> filtered {}, delaySeconds {};
    std::array<Ramp, 8> feedback;
    Ramp pole;
    std::size_t earlyPos = 0;
};

class Engine
{
public:
    void prepare(double rate, int room, float sizeValue, float tone)
    {
        sampleRate = rate;
        const float decay = std::exp2(2.0f * (sizeValue - 0.5f));
        // Preallocate all presets so selecting a room can never allocate on the audio thread.
        for (std::size_t i = 0; i < reverbs.size(); ++i)
            reverbs[i].prepare(rate, rooms[i], decay, tone);
        activeRoom = room; lastSize = sizeValue; decayMultiplier = decay;
        size.reset(rate, sizeValue);
    }
    void set(int room, float sizeValue, float tone, bool forceReset = false) noexcept
    {
        if (sizeValue != lastSize)
        {
            decayMultiplier = std::exp2(2.0f * (sizeValue - 0.5f));
            lastSize = sizeValue;
        }
        const bool changed = forceReset || room != activeRoom;
        if (changed)
        {
            activeRoom = room;
            reverbs[static_cast<std::size_t>(room)].reset();
            size.reset(sampleRate, sizeValue);
        }
        reverbs[static_cast<std::size_t>(room)].set(decayMultiplier, tone,
            rooms[static_cast<std::size_t>(room)], changed);
        size.set(sizeValue);
    }
    void reset() noexcept { for (auto& room : reverbs) room.reset(); }
    Stereo process(float left, float right) noexcept
    {
        const auto value = reverbs[static_cast<std::size_t>(activeRoom)].process(left, right);
        const float s = size.next();
        // A size impression from decay and preset-internal early/late voicing, with no
        // variable-delay motion, distance model, cross-room blend or user gain controls.
        const float earlyGain = 1.2f - 0.4f * s, lateGain = 0.65f + 0.7f * s;
        return {earlyGain * value.early.left + lateGain * value.late.left,
                earlyGain * value.early.right + lateGain * value.late.right};
    }
private:
    std::array<Room, 3> reverbs;
    Ramp size;
    double sampleRate = 48000;
    float lastSize = 0.5f, decayMultiplier = 1;
    int activeRoom = 0;
};
}

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
};
inline constexpr std::array<RoomSpec, 3> rooms {{
    { 0.48f, 3600.0f, {29.7f, 37.1f, 41.9f, 47.3f, 53.1f, 59.3f, 67.7f, 73.9f}, {4.1f, 7.3f, 11.9f, 17.3f, 23.1f, 31.7f} },
    { 0.85f, 6200.0f, {23.9f, 31.1f, 37.9f, 43.7f, 51.1f, 57.7f, 61.3f, 71.9f}, {2.7f, 5.3f, 8.9f, 13.7f, 19.3f, 27.1f} },
    { 1.35f, 9000.0f, {19.3f, 23.7f, 31.7f, 37.1f, 43.9f, 49.9f, 59.3f, 67.1f}, {1.9f, 3.7f, 6.1f, 9.7f, 15.1f, 21.1f} }
}};

class Room
{
public:
    void prepare(double rate, const RoomSpec& spec, float decay, float tone)
    {
        sampleRate = rate;
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
        constexpr std::array<float, 6> tapGain {0.30f, -0.23f, 0.19f, 0.15f, -0.12f, 0.09f};
        for (std::size_t i = 0; i < tapGain.size(); ++i)
        {
            const auto l = earlyPos >= tapsL[i] ? earlyPos - tapsL[i] : earlyPos + earlyL.size() - tapsL[i];
            const auto r = earlyPos >= tapsR[i] ? earlyPos - tapsR[i] : earlyPos + earlyR.size() - tapsR[i];
            early.left += tapGain[i] * (0.8f * earlyL[l] + 0.2f * earlyR[l]);
            early.right += tapGain[i] * (0.8f * earlyR[r] + 0.2f * earlyL[r]);
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
        Stereo late;
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            lines[i][positions[i]] = 0.20f * (injectionL[i] * left + injectionR[i] * right)
                + feedback[i].next() * (filtered[i] - 0.25f * sum);
            if (++positions[i] == lines[i].size()) positions[i] = 0;
            // Different readout vectors avoid a direct comb-like stereo image.
            late.left += y[i] * injectionR[i] * 0.25f;
            late.right += y[i] * injectionL[(i + 3) % 8] * 0.25f;
        }
        return {early, late};
    }
private:
    static float gain(float rt60, float delay) noexcept { return std::pow(10.0f, -3.0f * delay / rt60); }
    float coefficient(float hz) const noexcept
    { return std::exp(-6.28318530718f * std::min(hz, static_cast<float>(sampleRate * 0.45)) / static_cast<float>(sampleRate)); }
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
    void set(int room, float sizeValue, float tone) noexcept
    {
        if (sizeValue != lastSize)
        {
            decayMultiplier = std::exp2(2.0f * (sizeValue - 0.5f));
            lastSize = sizeValue;
        }
        const bool changed = room != activeRoom;
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

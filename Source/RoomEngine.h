#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>
#include "RoomCatalog.h"
#include "OutdoorEngine.h"
#include "AcousticGeometry.h"

namespace spatiallab
{
inline float finiteControl(float x) noexcept { return std::isfinite(x) ? std::clamp(x,0.0f,1.0f) : 0.5f; }

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
    std::array<float, 6> reflectionGains {};
    std::array<float, 6> earlyRightMs {}, reflectionGainsRight {};
};
// Retain only the established late-field delay distributions and timbre seeds.
struct RoomSeed {
 float cutoff;
 std::array<float,8> delaysMs;
 float crossfeed,diffusionAngle;
};
inline constexpr std::array<RoomSeed,19> roomSeeds {{
    {3400.0f, {23.3f, 29.9f, 34.7f, 39.1f, 43.7f, 48.7f, 53.3f, 59.9f}, 0.30f, 0.48f},
    {5200.0f, {13.7f, 17.9f, 21.1f, 25.3f, 29.9f, 33.7f, 37.1f, 41.9f}, 0.22f, 0.62f},
    {7600.0f, {9.7f, 12.1f, 14.9f, 17.3f, 20.9f, 23.9f, 27.7f, 31.1f}, 0.14f, 0.28f},
    {2800.0f, {17.3000f, 22.7000f, 27.1000f, 31.9000f, 36.7000f, 41.3000f, 47.9000f, 54.1000f}, 0.28f, 0.55f},
    {4600.0f, {31.1000f, 39.7000f, 47.3000f, 59.9000f, 67.1000f, 79.3000f, 89.9000f, 101.3000f}, 0.2f, 0.39f},
    {5100.0f, {41.3000f, 53.9000f, 67.1000f, 79.7000f, 97.3000f, 113.9000f, 131.3000f, 151.1000f}, 0.32f, 0.68f},
    {2900.0f, {23.9000f, 31.3000f, 43.7000f, 57.1000f, 69.7000f, 83.3000f, 97.9000f, 117.1000f}, 0.24f, 0.44f},
    {2400.0f, {19.1000f, 27.7000f, 34.1000f, 43.3000f, 52.9000f, 61.7000f, 73.1000f, 87.7000f}, 0.27f, 0.71f},
    {6200.0f, {29.3000f, 37.9000f, 46.1000f, 58.3000f, 71.9000f, 83.9000f, 97.1000f, 109.7000f}, 0.19f, 0.33f},
    {2400.0f, {2.3000f, 3.1000f, 4.7000f, 5.9000f, 7.1000f, 8.3000f, 9.7000f, 11.3000f}, 0.1f, 0.72f},
    {2900.0f, {3.1000f, 4.3000f, 5.7000f, 7.3000f, 8.9000f, 10.7000f, 12.1000f, 14.3000f}, 0.14f, 0.64f},
    {5100.0f, {5.3000f, 7.1000f, 9.7000f, 12.3000f, 15.1000f, 18.7000f, 22.1000f, 27.3000f}, 0.18f, 0.37f},
    {3600.0f, {3.7000f, 5.3000f, 6.7000f, 8.9000f, 11.3000f, 13.7000f, 16.1000f, 19.3000f}, 0.12f, 0.48f},
    {4800.0f, {53.3000f, 67.7000f, 83.9000f, 103.1000f, 127.3000f, 149.9000f, 173.3000f, 199.7000f}, 0.34f, 0.77f},
    {4200.0f, {89.3000f, 113.9000f, 139.7000f, 173.3000f, 211.1000f, 251.9000f, 293.3000f, 337.1000f}, 0.39f, 0.82f},
    {2700.0f, {47.9000f, 83.3000f, 113.9000f, 157.1000f, 197.3000f, 239.9000f, 283.7000f, 331.3000f}, 0.16f, 0.24f},
    {6500.0f, {2.7000f, 3.7000f, 5.3000f, 6.1000f, 7.9000f, 9.1000f, 10.3000f, 12.7000f}, 0.08f, 0.31f},
    {4300.0f, {3.3000f, 4.9000f, 6.3000f, 8.1000f, 10.9000f, 13.1000f, 15.7000f, 18.9000f}, 0.11f, 0.17f},
    {7800.0f, {1.7000f, 2.3000f, 3.1000f, 4.1000f, 5.3000f, 6.7000f, 8.1000f, 9.3000f}, 0.07f, 0.59f}
}};

// Keep the existing room-specific FDN line distribution; derive the neutral
// first-order field from documented source/receiver paths instead of signed taps.
inline const std::array<RoomSpec,19> rooms=[] {
 std::array<RoomSpec,19> result{};
 for(std::size_t i=0;i<result.size();++i){auto& s=result[i];const auto& g=roomGeometry[i];
  s.cutoff=roomSeeds[i].cutoff;s.delaysMs=roomSeeds[i].delaysMs;s.earlyCrossfeed=roomSeeds[i].crossfeed;s.diffusionAngle=roomSeeds[i].diffusionAngle;
  float absorptionArea=0,reflection=0;
  const std::array<float,6> area{g.depth*g.height,g.depth*g.height,g.width*g.height,g.width*g.height,g.width*g.depth,g.width*g.depth};
  for(int j=0;j<6;++j){const auto k=static_cast<std::size_t>(j);const auto p=firstOrderPath(g,j);
   s.earlyMs[k]=p.leftMs;s.earlyRightMs[k]=p.rightMs;s.reflectionGains[k]=p.leftGain;s.reflectionGainsRight[k]=p.rightGain;
   absorptionArea+=area[k]*g.absorption[k];reflection+=std::sqrt(1-g.absorption[k]);
  }
  // Sabine is only a design guide for bounding boxes, particularly cars/caves.
  s.rt60=std::clamp(.161f*g.width*g.depth*g.height/absorptionArea,.055f,g.decayCeiling);
  s.earlyLevel=1;s.lateLevel=.5f+reflection/6*(1-g.scatter);
 }
 return result;
}();

class Room
{
public:
    void prepare(double rate, const RoomSpec& spec, float decay, float tone)
    {
        sampleRate = rate;
        earlyScale = spec.earlyLevel; lateScale = spec.lateLevel; crossfeed = spec.earlyCrossfeed;
        reflectionGains = spec.reflectionGains; reflectionGainsRight=spec.reflectionGainsRight;
        scatterCos = std::cos(spec.diffusionAngle); scatterSin = std::sin(spec.diffusionAngle);
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            lines[i].assign(static_cast<std::size_t>(std::max(1.0, std::round(rate * spec.delaysMs[i] * 0.001))), 0.0f);
            positions[i] = 0;
            delaySeconds[i] = static_cast<float>(lines[i].size() / rate);
            feedback[i].reset(rate, gain(spec.rt60 * decay, delaySeconds[i]));
        }
        const float longest=std::max(*std::max_element(spec.earlyMs.begin(),spec.earlyMs.end()),*std::max_element(spec.earlyRightMs.begin(),spec.earlyRightMs.end()));
        earlyL.assign(static_cast<std::size_t>(std::max(rate*0.05,rate*(longest*1.071+0.37)*0.001))+2,0.0f);
        earlyR = earlyL;
        for (std::size_t i = 0; i < tapsL.size(); ++i)
        {
            tapsL[i] = static_cast<std::size_t>(std::round(rate * spec.earlyMs[i] * 0.001));
            tapsR[i] = static_cast<std::size_t>(std::round(rate * (spec.earlyRightMs[i]>0?spec.earlyRightMs[i]:spec.earlyMs[i] * 1.071f + 0.37f) * 0.001));
        }
        pole.reset(rate, coefficient(spec.cutoff * (std::exp2(2.0f * (tone - 0.5f)))));
        lastDecay = decay; lastTone = tone;
        filtered.fill(0.0f); earlyFiltered={}; earlyPos = 0;
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
        positions.fill(0); filtered.fill(0); earlyFiltered={}; earlyPos = 0;
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
            early.right += (reflectionGainsRight[i]!=0?reflectionGainsRight[i]:tapGain[i]) * ((1.0f - crossfeed) * earlyR[r] + crossfeed * earlyL[r]);
        }
        if (++earlyPos == earlyL.size()) earlyPos = 0;
        std::array<float, 8> y {};
        float sum = 0;
        const float p = pole.next();
        earlyFiltered[0]=(1-p)*early.left+p*earlyFiltered[0];
        earlyFiltered[1]=(1-p)*early.right+p*earlyFiltered[1];
        for(auto& v:earlyFiltered)if(std::abs(v)<1e-20f)v=0;
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
        return {{earlyFiltered[0] * earlyScale, earlyFiltered[1] * earlyScale},
                {late.left * lateScale, late.right * lateScale}};
    }
private:
    static float gain(float rt60, float delay) noexcept { return std::pow(10.0f, -3.0f * delay / rt60); }
    float coefficient(float hz) const noexcept
    { return std::exp(-6.28318530718f * std::min(hz, static_cast<float>(sampleRate * 0.45)) / static_cast<float>(sampleRate)); }
    float earlyScale = 1, lateScale = 1, crossfeed = 0.2f, scatterCos = 1, scatterSin = 0;
    std::array<float, 6> reflectionGains {}, reflectionGainsRight {};
    std::array<float,2> earlyFiltered {};
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
        rate = std::isfinite(rate) && rate >= 8000 && rate <= 384000 ? rate : 48000;
        room = std::clamp(room, 0, roomCount - 1); sizeValue = finiteControl(sizeValue); tone = finiteControl(tone);
        sampleRate = rate;
        const float decay = std::exp2(2.0f * (sizeValue - 0.5f));
        // Preallocate all presets so selecting a room can never allocate on the audio thread.
        for (std::size_t i = 0; i < reverbs.size(); ++i)
            reverbs[i].prepare(rate, rooms[i], decay, tone);
        outdoor.prepare(rate);
        if(isOutdoor(room)) outdoor.select(room-9,sizeValue,tone);
        activeRoom = room; lastSize = sizeValue; decayMultiplier = decay;
        size.reset(rate, sizeValue);
        pendingRoom=room;pendingSize=sizeValue;pendingTone=tone;switchGain=1;switching=false;fadeStep=static_cast<float>(1.0/(rate*0.03));
    }
    void set(int room, float sizeValue, float tone, bool forceReset = false) noexcept
    {
        room = std::clamp(room, 0, roomCount - 1); sizeValue = finiteControl(sizeValue); tone = finiteControl(tone);
        if (sizeValue != lastSize)
        {
            decayMultiplier = std::exp2(2.0f * (sizeValue - 0.5f));
            lastSize = sizeValue;
        }
        pendingRoom=room; pendingSize=sizeValue; pendingTone=tone;
        if(forceReset || room!=activeRoom)
        { switching=true; return; }
        if(!switching)
        {
            if(isOutdoor(activeRoom)) outdoor.set(sizeValue,tone);
            else reverbs[static_cast<std::size_t>(indoorIndex(activeRoom))].set(decayMultiplier,tone,rooms[static_cast<std::size_t>(indoorIndex(activeRoom))]);
            size.set(sizeValue);
        }
    }
    void reset() noexcept { for (auto& room : reverbs) room.reset(); outdoor.reset(); }
    Stereo process(float left, float right) noexcept
    {
        left = std::isfinite(left) ? left : 0.0f; right = std::isfinite(right) ? right : 0.0f;
        if(switching)
        {
            switchGain=std::max(0.0f,switchGain-fadeStep);
            if(switchGain==0)
            {
                activeRoom=pendingRoom;
                if(isOutdoor(activeRoom)) outdoor.select(activeRoom-9,pendingSize,pendingTone);
                else {
                    auto& next=reverbs[static_cast<std::size_t>(indoorIndex(activeRoom))]; next.reset();
                    next.set(std::exp2(2*(pendingSize-.5f)),pendingTone,rooms[static_cast<std::size_t>(indoorIndex(activeRoom))],true);
                }
                size.reset(sampleRate,pendingSize);switching=false;
            }
        }
        else switchGain=std::min(1.0f,switchGain+fadeStep);
        if(isOutdoor(activeRoom)) {const auto y=outdoor.process(left,right);return {switchGain*y[0],switchGain*y[1]};}
        const auto value = reverbs[static_cast<std::size_t>(indoorIndex(activeRoom))].process(left, right);
        const float s = size.next();
        // A size impression from decay and preset-internal early/late voicing, with no
        // variable-delay motion, distance model, cross-room blend or user gain controls.
        const float earlyGain = 1.2f - 0.4f * s, lateGain = 0.65f + 0.7f * s;
        return {switchGain*(earlyGain * value.early.left + lateGain * value.late.left),
                switchGain*(earlyGain * value.early.right + lateGain * value.late.right)};
    }
private:
    OutdoorEngine outdoor;
    std::array<Room, 19> reverbs;
    Ramp size;
    double sampleRate = 48000;
    float lastSize = 0.5f, decayMultiplier = 1;
    int activeRoom = 0,pendingRoom=0;
    float pendingSize=.5f,pendingTone=.5f,switchGain=1,fadeStep=1;
    bool switching=false;
};
}

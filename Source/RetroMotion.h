#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace spatiallab::retro
{
inline float randomUnit(std::uint32_t seed) noexcept
{
    seed ^= seed >> 16; seed *= 0x7feb352du; seed ^= seed >> 15;
    seed *= 0x846ca68bu; seed ^= seed >> 16;
    return static_cast<float>(seed & 0xffffu) / 65535.0f;
}
// GUI-only, deterministic and bounded. No random device, allocations or audio callbacks.
inline float light(double seconds, unsigned index, float audio, bool aged = true) noexcept
{
    const double offset = seconds + static_cast<double>(index) * 17.31;
    const auto cell = static_cast<std::uint32_t>(std::floor(offset * 0.7));
    const float fraction = static_cast<float>(offset * 0.7 - std::floor(offset * 0.7));
    const float smooth = fraction * fraction * (3 - 2 * fraction);
    const float noise = randomUnit(cell + index*31u)*(1-smooth) + randomUnit(cell+1u+index*31u)*smooth;
    const double cycleLength = 9.7 + index * 0.37;
    const auto cycle = static_cast<std::uint32_t>(std::floor(offset / cycleLength));
    const double moment = std::fmod(offset,cycleLength);
    // Rounded dip and slower recovery: no single-frame step or strobing.
    const float dip = aged && randomUnit(cycle+index*101u)>0.50f
        ? 0.30f*static_cast<float>(std::exp(-std::pow((moment-0.22)/0.13,2))) : 0.0f;
    const float variation = aged ? 1.0f : 0.72f;
    const float value = 0.58f + variation*(0.22f*static_cast<float>(std::sin(seconds*(0.83+index*0.063)+index))
        + (noise-0.5f)*0.14f) + std::clamp(audio,0.0f,1.0f)*0.15f - dip;
    return std::clamp(value,0.12f,0.96f);
}
inline float indicatorGate(double seconds, unsigned index) noexcept
{
    const float wave = static_cast<float>(std::sin(seconds*(0.59+index*0.041)+index*2.1));
    const float t = std::clamp((wave+0.2f)/0.45f,0.0f,1.0f);
    return 0.12f + 0.88f*t*t*(3-2*t);
}
}

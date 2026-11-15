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
// Dedicated thermionic glow: multi-second heat-up/cool-down, independent phase and period.
inline float tubeGlow(double seconds, unsigned index, bool aged) noexcept
{
    constexpr double periods[] {9.7, 12.1, 10.9, 14.3};
    constexpr double phases[] {0.4, 2.5, 4.4, 1.6};
    const auto i = index % 4u;
    const double angle = seconds * 6.283185307179586 / periods[i] + phases[i];
    const float heat = 0.5f + 0.5f * static_cast<float>(std::sin(angle));
    const float organic = static_cast<float>(std::sin(seconds * 1.31 + i * 2.7)) * (aged ? 0.025f : 0.01f);
    const double eventTime = std::fmod(seconds + i * 7.37, 19.1 + i * 1.7);
    const float flare = aged ? 0.09f * static_cast<float>(std::exp(-std::pow((eventTime-0.4)/0.15,2))) : 0.0f;
    return std::clamp(0.12f + heat * 0.77f + organic + flare,0.10f,0.96f);
}
}

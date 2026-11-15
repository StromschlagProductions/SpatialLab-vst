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
inline float light(double seconds, unsigned index, float audio) noexcept
{
    const double offset = seconds + static_cast<double>(index) * 17.31;
    const auto cell = static_cast<std::uint32_t>(std::floor(offset * 0.7));
    const float fraction = static_cast<float>(offset * 0.7 - std::floor(offset * 0.7));
    const float smooth = fraction * fraction * (3 - 2 * fraction);
    const float noise = randomUnit(cell + index * 31u) * (1 - smooth) + randomUnit(cell + 1u + index * 31u) * smooth;
    const auto cycle = static_cast<std::uint32_t>(std::floor(offset / 13.7));
    const double moment = std::fmod(offset, 13.7);
    const float flicker = moment < 0.09 && randomUnit(cycle + index * 101u) > 0.55f ? 0.12f : 0.0f;
    const float value = 0.50f + 0.065f * static_cast<float>(std::sin(seconds * (0.31 + index * 0.047) + index))
        + 0.025f * static_cast<float>(std::sin(seconds * 1.17 + index * 1.9))
        + (noise - 0.5f) * 0.055f + std::clamp(audio, 0.0f, 1.0f) * 0.09f - flicker;
    return std::clamp(value, 0.25f, 0.75f);
}
}

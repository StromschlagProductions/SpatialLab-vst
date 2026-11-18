#pragma once
#include <array>
namespace spatiallab {
// Fixed output-only voicing: no change to reflection/filter/feedback coefficients.
// +6 dB for nine ordinary indoor and seven outdoor presets; open land +18 dB
// retains its sparse, tail-free character but gives an audible low-level return.
inline constexpr float wetPlus6Db = 1.9952623149688795f;
inline constexpr float wetPlus18Db = 7.943282347242816f;
inline constexpr std::array<float,26> wetPresetGains {{
    wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db,
    wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db,
    wetPlus18Db, wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db, wetPlus6Db,
    1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f
}};
}

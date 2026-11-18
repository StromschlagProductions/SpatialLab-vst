#pragma once
#include <array>
#include <cmath>
namespace spatiallab_issue6_baseline {
struct OutdoorTap { float seconds, level, pan, cutoff, feedback; };
struct OutdoorProfile {
 std::array<OutdoorTap,16> taps{}; int count=0;
 bool valid() const noexcept {
  if(count<1||count>16)return false;
  float budget=0;
  for(int i=0;i<count;++i){const auto& t=taps[i];
   if(!std::isfinite(t.seconds)||!std::isfinite(t.level)||!std::isfinite(t.pan)||!std::isfinite(t.cutoff)||!std::isfinite(t.feedback)||t.seconds<.001f||t.seconds>1.49f||std::abs(t.level)>1||std::abs(t.pan)>1||t.cutoff<30||t.cutoff>20000)return false;
   budget+=std::abs(t.feedback);
  }
  return budget<=.43f;
 }
};
}

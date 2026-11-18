#pragma once
#include <array>
#include <cmath>
namespace spatiallab_v13 {
struct OutdoorTap {
 float seconds, level, pan, cutoff, feedback;
 float directMetres=0,reflectedMetres=0,absorption=0,scattering=0;
};
// Relative free-field pressure: absorption and scattering are energy fractions.
inline OutdoorTap surfacePath(float direct,float reflected,float pan,float cutoff,float absorption,float scattering) noexcept {
 return {(reflected-direct)/343,direct/reflected*std::sqrt((1-absorption)*(1-scattering)),pan,cutoff,0,direct,reflected,absorption,scattering};
}
struct OutdoorProfile {
 std::array<OutdoorTap,16> taps{}; int count=0;
 float diffuseRt60=0,diffuseLevel=0,diffuseCutoff=3000,diffuseScale=1;
 bool valid() const noexcept {
  if(count<1||count>16||!std::isfinite(diffuseRt60)||diffuseRt60<0||diffuseRt60>3||!std::isfinite(diffuseLevel)||diffuseLevel<0||diffuseLevel>1.5f||!std::isfinite(diffuseCutoff)||diffuseCutoff<30||diffuseCutoff>20000||!std::isfinite(diffuseScale)||diffuseScale<.25f||diffuseScale>3)return false;
  float budget=0;
  for(int i=0;i<count;++i){const auto& t=taps[i];
   if(!std::isfinite(t.seconds)||!std::isfinite(t.level)||!std::isfinite(t.pan)||!std::isfinite(t.cutoff)||!std::isfinite(t.feedback)||t.seconds<.001f||t.seconds>1.49f||std::abs(t.level)>1||std::abs(t.pan)>1||t.cutoff<30||t.cutoff>20000)return false;
   budget+=std::abs(t.feedback);
  }
  return budget<=.43f;
 }
};
}

#pragma once
#include <array>
#include "OutdoorPlan.h"
#include <vector>
#include <algorithm>
#include <cmath>
namespace spatiallab_issue6_baseline {
inline constexpr std::array<OutdoorProfile,7> outdoorProfiles {{
 {{{{.19f,.018f,-.7f,2400,0},{.43f,.009f,.6f,1700,0}}},2},
 {{{{.023f,.09f,-.8f,3700,0},{.041f,.08f,.6f,2900,0},{.067f,-.06f,-.2f,2300,0},{.103f,.055f,.9f,1800,0},{.149f,.04f,-.65f,1600,0},{.213f,-.03f,.35f,1300,0},{.307f,.018f,-.4f,1100,0},{.389f,.012f,.7f,900,0}}},8},
 {{{{.019f,.22f,-.9f,6800,0},{.037f,.18f,.85f,5900,0},{.071f,-.11f,-.7f,4700,0},{.113f,.07f,.65f,3600,0}}},4},
 {{{{.031f,.19f,-.8f,6200,.07f},{.053f,.17f,.75f,5500,.06f},{.089f,.13f,-.25f,4700,.05f},{.137f,.10f,.5f,3900,.04f},{.193f,.06f,-.6f,3100,.03f},{.269f,.035f,.3f,2400,.02f}}},6},
 {{{{.31f,.27f,-.8f,4100,.12f},{.57f,.19f,.8f,3100,.09f},{.89f,.10f,-.4f,2200,.05f},{1.21f,.055f,.5f,1700,.02f}}},4},
 {{{{.027f,.20f,-.6f,6500,.12f},{.054f,-.17f,.6f,5800,.10f},{.083f,.14f,-.5f,5100,.08f},{.117f,.11f,.5f,4400,.06f},{.157f,-.08f,-.3f,3600,.04f},{.211f,.05f,.3f,2900,.02f}}},6},
 {{{{.007f,.29f,0,7800,.06f},{.017f,.16f,-.85f,6400,.04f},{.029f,-.13f,.8f,5300,.03f},{.047f,.08f,-.4f,4400,.02f},{.073f,.045f,.4f,3400,0}}},5}
}};
// Shared stereo delay bus. The sum of absolute feedback gains is <= .42.
// Linear pan and convex read-head fades cannot increase the bus infinity norm.
class OutdoorEngine {
public:
 void prepare(double sampleRate) { rate=sampleRate; buffer.assign(static_cast<std::size_t>(rate*3)+4,{}); blendStep=float(1/(rate*.05)); smooth=float(1-std::exp(-1/(rate*.05))); reset(); }
 void reset() noexcept { std::fill(buffer.begin(),buffer.end(),std::array<float,2>{}); filtered={};position=0; }
 void select(int index,float size,float tone) noexcept { profile=index;reset(); currentSize=targetSize=nextSize=std::exp2(2*(size-.5f)); mix=1;lastSize=size;lastTone=-1;set(size,tone);poles=targetPoles; }
 void set(float size,float tone) noexcept {
  if(size!=lastSize){nextSize=std::exp2(2*(size-.5f));lastSize=size;}
  if(tone!=lastTone){lastTone=tone;for(int i=0;i<outdoorProfiles[profile].count;++i){float hz=std::min(float(rate*.45),outdoorProfiles[profile].taps[i].cutoff*std::exp2(2*(tone-.5f)));targetPoles[i]=float(std::exp(-6.28318530718*hz/rate));}}
 }
 std::array<float,2> process(float left,float right) noexcept {
  if(mix>=1 && nextSize!=targetSize){currentSize=targetSize;targetSize=nextSize;mix=0;}
  mix=std::min(1.f,mix+blendStep);
  std::array<float,2> output{},feedback{};
  const auto& p=outdoorProfiles[profile];
  for(int i=0;i<p.count;++i){const auto& t=p.taps[i];poles[i]+=smooth*(targetPoles[i]-poles[i]);
   const float dl=float(rate)*t.seconds*currentSize,dr=float(rate)*t.seconds*targetSize;
   for(int c=0;c<2;++c){float x=.75f*read(dr,c)+.25f*read(dr,1-c); if(mix<1) x=(1-mix)*(.75f*read(dl,c)+.25f*read(dl,1-c))+mix*x;filtered[i][c]=(1-poles[i])*x+poles[i]*filtered[i][c];
    if(std::abs(filtered[i][c])<1e-20f)filtered[i][c]=0;
    const float pan=c==0?(1-t.pan)*.5f:(1+t.pan)*.5f;
    // Preserve stereo excitation while distributing each surface between the two sides.
    float v=filtered[i][c]*pan;output[c]+=t.level*v;feedback[c]+=t.feedback*v;}
  }
  buffer[position]={left+feedback[0],right+feedback[1]}; if(++position==buffer.size())position=0;return output;
 }
private:
 float read(float delay,int c) const noexcept {delay=std::clamp(delay,1.f,float(buffer.size()-2));double at=double(position)-double(delay);if(at<0)at+=double(buffer.size());auto integral=static_cast<std::size_t>(at);auto a=integral;auto b=a+1;if(b==buffer.size())b=0;float f=float(at-double(integral));return buffer[a][c]*(1-f)+buffer[b][c]*f;}
 std::vector<std::array<float,2>> buffer;
 std::array<std::array<float,2>,16> filtered{};
 std::array<float,16> poles{},targetPoles{};
 std::size_t position=0;double rate=48000;
 int profile=0;float currentSize=1,targetSize=1,nextSize=1,mix=1,blendStep=1,smooth=1,lastSize=.5f,lastTone=-1;
};
}

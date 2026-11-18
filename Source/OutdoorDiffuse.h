#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
namespace spatiallab {
// Four-line passive Householder field, driven only by reflected energy.
// All storage is prepared up front; automation changes coefficients, not memory.
class OutdoorDiffuse {
public:
 void prepare(double sampleRate){rate=sampleRate;for(auto& line:lines)line.assign(static_cast<std::size_t>(rate*.1)+2,0);smooth=float(1-std::exp(-1/(rate*.05)));}
 void reset() noexcept {for(auto& line:lines)std::fill(line.begin(),line.end(),0);position={};filtered={};}
 void select(float rt60,float cutoff,float scale) noexcept {
  reset();constexpr std::array<float,4> ms{11.3f,17.9f,23.7f,31.1f};
  for(std::size_t i=0;i<4;++i)length[i]=std::clamp<std::size_t>(static_cast<std::size_t>(std::round(rate*ms[i]*scale*.001)),1,lines[i].size());
  set(rt60,cutoff);gain=targetGain;pole=targetPole;
 }
 void set(float rt60,float cutoff) noexcept {
  targetPole=float(std::exp(-6.28318530718*std::min(double(cutoff),rate*.45)/rate));
  for(std::size_t i=0;i<4;++i)targetGain[i]=rt60>0?std::min(.995f,float(std::pow(10.,-3*double(length[i])/rate/rt60))):0;
 }
 std::array<float,2> process(float l,float r) noexcept {
  std::array<float,4> y{};float sum=0;pole+=smooth*(targetPole-pole);
  for(std::size_t i=0;i<4;++i){y[i]=lines[i][position[i]];filtered[i]=(1-pole)*y[i]+pole*filtered[i];if(std::abs(filtered[i])<1e-20f)filtered[i]=0;sum+=filtered[i];}
  const std::array<float,4> input{l+r,l-r,-l+r,-l-r};
  for(std::size_t i=0;i<4;++i){gain[i]+=smooth*(targetGain[i]-gain[i]);lines[i][position[i]]=.25f*input[i]+gain[i]*(filtered[i]-.5f*sum);if(++position[i]==length[i])position[i]=0;}
  return {.5f*(y[0]+y[1]-y[2]+y[3]),.5f*(y[0]-y[1]+y[2]+y[3])};
 }
private:
 std::array<std::vector<float>,4> lines;
 std::array<std::size_t,4> length{1,1,1,1},position{};
 std::array<float,4> filtered{},gain{},targetGain{};
 double rate=48000;float smooth=1,pole=0,targetPole=0;
};
}

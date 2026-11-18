#pragma once
#include <array>
#include "OutdoorPlan.h"
#include "OutdoorDiffuse.h"
#include <vector>
#include <algorithm>
#include <cmath>
namespace spatiallab_v13 {
// Authored paths at a 2-m source/receiver reference (canyon: 4 m). Units and assumptions are
// exported by the audit tool; this is not a claim to measured site replication.
inline const std::array<OutdoorProfile,7> outdoorProfiles=[] {
 std::array<OutdoorProfile,7> p{};
 const auto path=[&](int scene,float reflected,float pan,float hz,float absorption,float scatter){auto& v=p[static_cast<std::size_t>(scene)];v.taps[static_cast<std::size_t>(v.count++)]=surfacePath(scene==4?4.f:2.f,reflected,pan,hz,absorption,scatter);};
 const auto wall=[](float offset){return std::sqrt(4+4*offset*offset);};
 // Soft ground plus one remote boundary; no diffuse tail in open country.
 path(0,wall(1.5f),-.12f,1900,.98f,.80f);path(0,42,.18f,1300,.65f,.80f);
 // Deterministic irregular tree scattering; no random state/audio-thread RNG.
 const std::array<float,16> forestPaths{5.4f,7.1f,8.8f,10.9f,13.3f,16.7f,20.4f,24.8f,29.6f,35.1f,41.7f,49.3f,58.1f,67.4f,78.9f,92.3f};
 const std::array<float,16> forestPan{-.6f,.25f,-.2f,.7f,.1f,-.75f,.45f,-.35f,.65f,-.1f,-.55f,.3f,.8f,-.4f,.2f,-.65f};
 for(std::size_t i=0;i<16;++i)path(1,forestPaths[i],forestPan[i],4200/(1+float(i)*.14f),.45f,.60f);
 p[1].diffuseRt60=.28f;p[1].diffuseLevel=.35f;p[1].diffuseCutoff=2100;p[1].diffuseScale=.8f;
 // Street: ground, two facades, recessed/end faces and higher-order paths.
 for(const auto& v:std::array<std::array<float,3>,8>{{{wall(1.5f),-.05f,.16f},{wall(3.4f),-.7f,.18f},{wall(4.6f),.65f,.22f},{18.4f,-.35f,.30f},{25.3f,.4f,.35f},{31.7f,-.2f,.42f},{39.2f,.5f,.45f},{48.6f,-.4f,.50f}}})path(2,v[0],v[1],6200,v[2],.2f);
 p[2].diffuseRt60=.38f;p[2].diffuseLevel=.50f;p[2].diffuseCutoff=3700;
 for(int i=0;i<12;++i){const std::array<float,12> lengths{wall(1.5f),wall(5),wall(7),18.1f,22.7f,27.3f,33.9f,39.6f,47.1f,54.7f,63.3f,75.8f};path(3,lengths[static_cast<std::size_t>(i)],forestPan[static_cast<std::size_t>(i)]*.8f,5400/(1+float(i)*.05f),.18f+float(i)*.025f,.23f);}
 p[3].diffuseRt60=.68f;p[3].diffuseLevel=.75f;p[3].diffuseCutoff=3500;p[3].diffuseScale=1.3f;
 // Near irregular rock returns, then two weaker far contours. No tap feedback
 // and no fixed alternating L/R sequence or regular echo spacing.
 const std::array<float,10> rockPaths{std::sqrt(160.f),15.8f,18.9f,std::sqrt(500.f),27.4f,34.7f,43.8f,52.3f,66.4f,116.7f};
 const std::array<float,10> rockPan{-.45f,-.25f,-.38f,.3f,.16f,-.12f,-.4f,.22f,-.35f,-.18f};
 for(std::size_t i=0;i<10;++i)path(4,rockPaths[i],rockPan[i],4600/(1+float(i)*.12f),.10f,.25f+float(i)*.025f);
 p[4].diffuseRt60=.55f;p[4].diffuseLevel=.3f;p[4].diffuseCutoff=2600;p[4].diffuseScale=1.5f;
 // Tunnel: 6-m width, 4.2-m height, 1.5-m source/receiver height.
 const std::array<float,12> tunnelPaths{wall(1.5f),wall(2.7f),wall(2.6f),wall(3.4f),wall(5.2f),wall(6.8f),wall(8.1f),wall(9.7f),wall(12.4f),wall(15.3f),wall(18.7f),wall(23.1f)};
 for(std::size_t i=0;i<12;++i)path(5,tunnelPaths[i],forestPan[i]*.65f,6500/(1+float(i)*.055f),.10f+float(i)*.025f,.15f);
 p[5].diffuseRt60=1.15f;p[5].diffuseLevel=1.0f;p[5].diffuseCutoff=4200;p[5].diffuseScale=1.2f;
 // Short open underpass: hard floor/soffit and interrupted lateral faces.
 const std::array<float,9> underPaths{wall(1.5f),wall(1.6f),wall(3.1f),wall(4.3f),wall(5.7f),16.9f,22.7f,28.1f,36.6f};
 for(std::size_t i=0;i<9;++i)path(6,underPaths[i],forestPan[i]*.6f,6800/(1+float(i)*.06f),.12f+float(i)*.04f,.20f);
 p[6].diffuseRt60=.38f;p[6].diffuseLevel=.6f;p[6].diffuseCutoff=4300;p[6].diffuseScale=.7f;
 return p;
}();
// Shared stereo delay bus. The sum of absolute feedback gains is <= .42.
// Linear pan and convex read-head fades cannot increase the bus infinity norm.
class OutdoorEngine {
public:
 void prepare(double sampleRate) { rate=sampleRate; buffer.assign(static_cast<std::size_t>(rate*3)+4,{}); blendStep=float(1/(rate*.05)); smooth=float(1-std::exp(-1/(rate*.05))); diffuse.prepare(rate);reset(); }
 void reset() noexcept { std::fill(buffer.begin(),buffer.end(),std::array<float,2>{}); filtered={};position=0;diffuse.reset(); }
 void select(int index,float size,float tone) noexcept { profile=index;reset();
  for(int i=0;i<outdoorProfiles[static_cast<std::size_t>(profile)].count;++i){float p=outdoorProfiles[static_cast<std::size_t>(profile)].taps[static_cast<std::size_t>(i)].pan;panGains[static_cast<std::size_t>(i)]={std::sqrt((1-p)*.5f),std::sqrt((1+p)*.5f)};} currentSize=targetSize=nextSize=std::exp2(2*(size-.5f)); mix=1;lastSize=size;lastTone=-1;set(size,tone);poles=targetPoles;const auto& p=outdoorProfiles[static_cast<std::size_t>(profile)];diffuse.select(p.diffuseRt60*currentSize,p.diffuseCutoff*std::exp2(2*(tone-.5f)),p.diffuseScale); }
 void set(float size,float tone) noexcept {
  const bool changed=size!=lastSize||tone!=lastTone;
  if(size!=lastSize){nextSize=std::exp2(2*(size-.5f));lastSize=size;}
  if(tone!=lastTone){lastTone=tone;for(int i=0;i<outdoorProfiles[profile].count;++i){float hz=std::min(float(rate*.45),outdoorProfiles[profile].taps[i].cutoff*std::exp2(2*(tone-.5f)));targetPoles[i]=float(std::exp(-6.28318530718*hz/rate));}}
  if(changed){const auto& p=outdoorProfiles[static_cast<std::size_t>(profile)];diffuse.set(p.diffuseRt60*nextSize,p.diffuseCutoff*std::exp2(2*(tone-.5f)));}
 }
 std::array<float,2> process(float left,float right) noexcept {
  if(mix>=1 && nextSize!=targetSize){currentSize=targetSize;targetSize=nextSize;mix=0;}
  mix=std::min(1.f,mix+blendStep);
  std::array<float,2> output{},feedback{};
  const auto& p=outdoorProfiles[profile];
  for(int i=0;i<p.count;++i){const auto& t=p.taps[i];poles[i]+=smooth*(targetPoles[i]-poles[i]);
   const float dl=float(rate)*t.seconds*currentSize,dr=float(rate)*t.seconds*targetSize;
   const float targetL=read(dr,0),targetR=read(dr,1);
   std::array<float,2> excitation{.75f*targetL+.25f*targetR,.75f*targetR+.25f*targetL};
   if(mix<1){const float oldL=read(dl,0),oldR=read(dl,1);excitation[0]=(1-mix)*(.75f*oldL+.25f*oldR)+mix*excitation[0];excitation[1]=(1-mix)*(.75f*oldR+.25f*oldL)+mix*excitation[1];}
   for(int c=0;c<2;++c){float x=excitation[static_cast<std::size_t>(c)];filtered[i][c]=(1-poles[i])*x+poles[i]*filtered[i][c];
    if(std::abs(filtered[i][c])<1e-20f)filtered[i][c]=0;
    const float pan=panGains[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)];
    // Preserve stereo excitation while distributing each surface between the two sides.
    float v=filtered[i][c]*pan;output[c]+=t.level*v;feedback[c]+=t.feedback*v;}
  }
  buffer[position]={left+feedback[0],right+feedback[1]}; if(++position==buffer.size())position=0;
  if(p.diffuseLevel>0){const auto late=diffuse.process(output[0],output[1]);output[0]+=p.diffuseLevel*late[0];output[1]+=p.diffuseLevel*late[1];}
  return output;
 }
private:
 float read(float delay,int c) const noexcept {delay=std::clamp(delay,1.f,float(buffer.size()-2));double at=double(position)-double(delay);if(at<0)at+=double(buffer.size());auto integral=static_cast<std::size_t>(at);auto a=integral;auto b=a+1;if(b==buffer.size())b=0;float f=float(at-double(integral));return buffer[a][c]*(1-f)+buffer[b][c]*f;}
 OutdoorDiffuse diffuse;
 std::vector<std::array<float,2>> buffer;
 std::array<std::array<float,2>,16> filtered{},panGains{};
 std::array<float,16> poles{},targetPoles{};
 std::size_t position=0;double rate=48000;
 int profile=0;float currentSize=1,targetSize=1,nextSize=1,mix=1,blendStep=1,smooth=1,lastSize=.5f,lastTone=-1;
};
}

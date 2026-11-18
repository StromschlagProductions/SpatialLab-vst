#include "RoomEngine.h"
#include <atomic>
#include <cstdlib>
#include <new>
#include <iostream>
#include <stdexcept>
static std::atomic<bool> watch{false};static std::atomic<int> allocations{0};
void* operator new(std::size_t n){if(watch.load())++allocations;if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
static void require(bool v,const char* message){if(!v)throw std::runtime_error(message);}
int main(){try{
 for(std::size_t i=0;i<spatiallab::roomGeometry.size();++i){const auto& g=spatiallab::roomGeometry[i];const auto& s=spatiallab::rooms[i];
  require(g.direct>0 && g.depth*.25f+g.direct<g.depth && g.heightSource<g.height,"Invalid authored source/receiver geometry");
  for(std::size_t j=0;j<6;++j)require(s.earlyMs[j]>0 && s.earlyRightMs[j]>0 && s.reflectionGains[j]>=0 && s.reflectionGains[j]<1 && s.reflectionGainsRight[j]<1,"Nonpassive first-order surface");
 }
 for(const auto& p:spatiallab::outdoorProfiles){require(p.valid(),"Invalid outdoor model");
  for(int i=0;i<p.count;++i){const auto& t=p.taps[static_cast<std::size_t>(i)];require(t.reflectedMetres>t.directMetres && std::abs(t.seconds-(t.reflectedMetres-t.directMetres)/343)<1e-6,"Outdoor delay is not a path difference");}
 }
 for(int rate:{44100,48000,96000})for(int room=0;room<26;++room){
  // Repeated unchanged host targets at different block boundaries must be bit-identical.
  spatiallab::Engine a,b;a.prepare(rate,room,.5f,.5f);b.prepare(rate,room,.5f,.5f);double energy=0;
  for(int i=0;i<rate/8;++i){if(i%127==0)a.set(room,.5f,.5f);if(i%7==0)b.set(room,.5f,.5f);float l=i==0?1.f:0.f;auto x=a.process(l,l),y=b.process(l,l);require(x.left==y.left&&x.right==y.right,"Block schedule changed response");require(std::isfinite(x.left)&&std::isfinite(x.right),"Nonfinite IR");if(i==0)require(x.left==0&&x.right==0,"Direct signal leaked");energy+=double(x.left)*x.left+double(x.right)*x.right;}
  require(energy>1e-8,"Silent environment");
  for(float size:{0.f,.5f,1.f})for(float tone:{0.f,.5f,1.f}){
   spatiallab::Engine e;e.prepare(rate,room,size,tone);float previous=0;double power=0;
   watch=true;
   for(int i=0;i<rate/10;++i){if(i%64==0)e.set(room,size,tone);float x=.04f*std::sin(float(i)*.031f);auto y=e.process(x,-.7f*x);
    if(!std::isfinite(y.left)||!std::isfinite(y.right)||std::abs(y.left)>4||std::abs(y.right)>4){watch=false;throw std::runtime_error("Extreme control instability");}
    if(std::abs(y.left-previous)>.15f){watch=false;throw std::runtime_error("Unexpected output discontinuity");}previous=y.left;power+=y.left*y.left+y.right*y.right;
   }
   // Includes transitions into/out of the diffuse outdoor variants under allocation watch.
   for(int next=0;next<26;++next){e.set(next,1-size,1-tone);for(int i=0;i<rate/16;++i){auto y=e.process(0,0);if(!std::isfinite(y.left)||!std::isfinite(y.right)||std::abs(y.left)>4||std::abs(y.right)>4){watch=false;throw std::runtime_error("Unstable preset transition");}}}
   watch=false;require(power>0,"Stereo source lost");require(allocations.load()==0,"Audio path allocated");
  }
 }
 std::cout<<"PASS: 26 calibrated environments, 44.1/48/96 kHz, macro corners, deterministic blocks, wet-only, stereo, bounded transitions, zero audio allocations\n";
}catch(const std::exception& e){watch=false;std::cerr<<e.what()<<'\n';return 1;}}

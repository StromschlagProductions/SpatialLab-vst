#include "RoomEngine.h"
#include "Reference/V1_3/RoomEngine.h"
#include <iostream>
#include <stdexcept>
static void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{
 for(int rate:{44100,48000})for(int room=0;room<26;++room)for(float size:{0.f,.5f,1.f})for(float tone:{0.f,.5f,1.f}){
  spatiallab::Engine current;spatiallab_v13::Engine before;
  current.prepare(rate,room,size,tone);before.prepare(rate,room,size,tone);
  const float gain=room==9?7.943282347242816f:room<16?1.9952623149688795f:1.f;
  require(spatiallab::wetPresetGains[static_cast<std::size_t>(room)]==gain,"Incorrect fixed preset gain");
  double a=0,b=0;
  for(int i=0;i<rate/2;++i){
   // Correlated + side-only components exercise both channels without mono summing.
   const float l=i==0?.1f:.025f*std::sin(float(i)*.031f),r=i==0?0.f:.019f*std::sin(float(i)*.047f);
   auto x=before.process(l,r);auto y=current.process(l,r);
   require(std::isfinite(y.left)&&std::isfinite(y.right),"Nonfinite wet output");
   require(y.left==x.left*gain && y.right==x.right*gain,"Acoustics changed beyond the authorized scalar gain");
   if(room>=16)require(y.left==x.left && y.right==x.right,"Protected special/vehicle/cockpit audio changed");
   if(i==0)require(y.left==0 && y.right==0,"Dry leakage");
   a+=double(x.left)*x.left+double(x.right)*x.right;b+=double(y.left)*y.left+double(y.right)*y.right;
  }
  require(a>1e-12,"Silent preset");const double db=10*std::log10(b/a),target=room==9?18:room<16?6:0;
  require(std::abs(db-target)<.00001,"Measured gain is not the specified dB change");
 }
 for(int rate:{44100,48000})for(int first=0;first<26;++first){
  spatiallab::Engine e;e.prepare(rate,first,.5f,.5f);float last=0;
  for(int next=0;next<26;++next){e.set(next,.5f,.5f);for(int i=0;i<rate/10;++i){
   float input=.01f*std::sin(float(i)*.021f);auto y=e.process(input,input);
   require(std::isfinite(y.left)&&std::abs(y.left)<.25f&&std::abs(y.right)<.25f,"Transition headroom failure");
   require(std::abs(y.left-last)<.02f,"Preset gain changed with an abrupt discontinuity");last=y.left;
  }}
 }
 std::cout<<"PASS 26 presets at 44.1/48 kHz: exact +6/+18/0 dB, all macro corners, stereo, protected audio bit-identical, 676 smooth transitions per rate\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

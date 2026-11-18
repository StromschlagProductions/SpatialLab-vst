#include "RoomEngine.h"
#include "RoomCatalog.h"
#include "OutdoorPlan.h"
#include "Reference/RoomEnginePhase3A.h"
#include "Reference/Issue6/RoomEngine.h"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <stdexcept>
void need(bool v,const char* m){if(!v)throw std::runtime_error(m);}
void le(std::ostream& s,std::uint32_t x,int n=4){for(int i=0;i<n;++i)s.put(static_cast<char>((x>>(8*i))&255));}
int main(int argc,char** argv)
{
 try {
  auto plan=spatiallab::outdoorProfiles[0];need(plan.valid(),"Invalid outdoor contract");plan.count=17;need(!plan.valid(),"Unbounded outdoor contract");
  if(argc>1)std::filesystem::create_directories(argv[1]);
  need(spatiallab::normalizedRoom(1e30f)==25 && spatiallab::normalizedRoom(-1e30f)==0,"Host bounds");
  for(int i=0;i<26;++i)need(spatiallab::normalizedRoom(spatiallab::roomNormalized(i))==i,"Room index not bijective");
  for(const auto& p:spatiallab::outdoorProfiles){need(p.valid(),"Invalid outdoor profile");double feedback=0;for(int i=0;i<p.count;++i)feedback+=std::abs(p.taps[i].feedback);need(feedback<=.43,"Feedback budget exceeded");}
  std::cout<<"rate,index,key,backend,energy,T20_RT60,peak,stereo_difference,hash\n";
  for(int rate:{44100,48000})
  {
   for(int old=0;old<19;++old){int room=old<9?old:old+7;spatiallab_issue6_baseline::Engine now;spatiallab_phase3a::Engine before;now.prepare(rate,room,.5f,.5f);before.prepare(rate,old,.5f,.5f);for(int i=0;i<rate;++i){float x=i==0?1.f:0.f;auto a=now.process(x,x);auto b=before.process(x,x);need(a.left==b.left&&a.right==b.right,"Frozen issue-6 baseline no longer matches approved legacy rooms");}}
   std::set<std::uint64_t> signatures;
   for(int room=0;room<26;++room)
   {
    spatiallab::Engine neutral,small,dark,large;
    neutral.prepare(rate,room,.5f,.5f);small.prepare(rate,room,0,.5f);dark.prepare(rate,room,.5f,0);large.prepare(rate,room,1,.5f);
    const bool outdoor=spatiallab::isOutdoor(room);const auto& spec=spatiallab::rooms[static_cast<std::size_t>(outdoor?0:spatiallab::indoorIndex(room))];const int length=static_cast<int>(rate*(outdoor?16:(spec.rt60*2+1)));
    std::vector<spatiallab::Stereo> ir;ir.reserve(static_cast<std::size_t>(length));
    std::uint64_t hash=1469598103934665603ull;double energy=0,stereo=0,peak=0,smallDifference=0,darkDifference=0,largeDifference=0;
    for(int i=0;i<length;++i)
    {
     float x=i==0?1.f:0.f;auto a=neutral.process(x,x),b=small.process(x,x),c=dark.process(x,x),d=large.process(x,x);
     for(float v:{a.left,a.right,b.left,b.right,c.left,c.right,d.left,d.right})need(std::isfinite(v)&&std::abs(v)<3,"Invalid/unstable response");
     if(i==0)need(a.left==0&&a.right==0,"Wet contains direct input");
     for(float v:{a.left,a.right}){std::uint32_t bits;std::memcpy(&bits,&v,4);hash^=bits;hash*=1099511628211ull;peak=std::max(peak,double(std::abs(v)));}
     energy+=double(a.left)*a.left+double(a.right)*a.right;stereo+=std::abs(a.left-a.right);
     smallDifference+=std::abs(a.left-b.left);darkDifference+=std::abs(a.left-c.left);largeDifference+=std::abs(a.left-d.left);ir.push_back(a);
    }
    need(energy>1e-5&&stereo>1e-5,"Missing room/stereo response");need(smallDifference>1e-5&&largeDifference>1e-5&&darkDifference>1e-5,"Macro has no effect");need(signatures.insert(hash).second,"Duplicate room IR");
    double tail=0,t5=-1,t25=-1;
    for(int i=length;i-->0;){tail+=double(ir[i].left)*ir[i].left+double(ir[i].right)*ir[i].right;if(tail>=energy*std::pow(10.,-2.5)&&t25<0)t25=i/double(rate);if(tail>=energy*std::pow(10.,-.5)&&t5<0)t5=i/double(rate);}
    double ending=0;for(int i=length-rate/4;i<length;++i)ending+=double(ir[i].left)*ir[i].left+double(ir[i].right)*ir[i].right;need(ending<energy*1e-6,"Room does not decay naturally");
    std::cout<<rate<<','<<room<<','<<spatiallab::roomKeys[room]<<','<<(outdoor?"outdoor":"indoor")<<','<<energy<<','<<3*(t25-t5)<<','<<peak<<','<<stereo<<','<<hash<<'\n';
    if(argc>1){std::ofstream out(std::filesystem::path(argv[1])/(std::to_string(rate)+"-"+spatiallab::roomKeys[room]+".wav"),std::ios::binary);out.write("RIFF",4);le(out,36+length*8);out.write("WAVEfmt ",8);le(out,16);le(out,3,2);le(out,2,2);le(out,rate);le(out,rate*8);le(out,8,2);le(out,32,2);out.write("data",4);le(out,length*8);for(auto v:ir)for(float x:{v.left,v.right}){std::uint32_t bits;std::memcpy(&bits,&x,4);le(out,bits);}need(out.good(),"IR write failed");}
   }
  }
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}

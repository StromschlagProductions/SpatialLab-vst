#include "AuditFpMode.h"
// Offline renderer of the production Engine. No substitute reverb or normalization.
#if defined(SPATIALLAB_AUDIT_BASELINE)
#include "Reference/Issue6/RoomEngine.h"
namespace audit=spatiallab_issue6_baseline;
#else
#include "RoomEngine.h"
namespace audit=spatiallab;
#endif
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
namespace fs=std::filesystem;
int main(int argc,char** argv) {
 try {
  AuditFpMode floatingPointMode;
  if(argc<2)throw std::runtime_error("Usage: audit output-dir [mono-float32-48k-source]");
  fs::create_directories(argv[1]);
  std::vector<float> speech;
  if(argc>2){std::ifstream s(argv[2],std::ios::binary|std::ios::ate);if(!s)throw std::runtime_error("Missing source");auto bytes=s.tellg();speech.resize(static_cast<std::size_t>(bytes)/4);s.seekg(0);s.read(reinterpret_cast<char*>(speech.data()),bytes);}
#if !defined(SPATIALLAB_AUDIT_BASELINE)
  std::ofstream model(fs::path(argv[1])/"model-paths.csv");model<<std::setprecision(9);
  model<<"key,field,tap,width_m,depth_m,height_m,direct_m,reflected_left_m,reflected_right_m,delay_left_ms,delay_right_ms,gain_left,gain_right,absorption,scattering,cutoff_hz,nominal_rt60_s,late_level\n";
  for(int room=0;room<audit::roomCount;++room){
   if(audit::isOutdoor(room)){const auto& p=audit::outdoorProfiles[static_cast<std::size_t>(room-9)];
    for(int i=0;i<p.count;++i){const auto& t=p.taps[static_cast<std::size_t>(i)];model<<audit::roomKeys[static_cast<std::size_t>(room)]<<",outdoor,"<<i<<",,,,"<<t.directMetres<<','<<t.reflectedMetres<<','<<t.reflectedMetres<<','<<t.seconds*1000<<','<<t.seconds*1000<<','<<t.level*std::sqrt((1-t.pan)*.5f)<<','<<t.level*std::sqrt((1+t.pan)*.5f)<<','<<t.absorption<<','<<t.scattering<<','<<t.cutoff<<','<<p.diffuseRt60<<','<<p.diffuseLevel<<'\n';}
   }else{auto k=static_cast<std::size_t>(audit::indoorIndex(room));const auto& g=audit::roomGeometry[k];const auto& s=audit::rooms[k];
    for(std::size_t i=0;i<6;++i)model<<audit::roomKeys[static_cast<std::size_t>(room)]<<",indoor,"<<i<<','<<g.width<<','<<g.depth<<','<<g.height<<','<<g.direct<<','<<g.direct+s.earlyMs[i]*.343f<<','<<g.direct+s.earlyRightMs[i]*.343f<<','<<s.earlyMs[i]<<','<<s.earlyRightMs[i]<<','<<s.reflectionGains[i]<<','<<s.reflectionGainsRight[i]<<','<<g.absorption[i]<<','<<g.scatter<<','<<s.cutoff<<','<<s.rt60<<','<<s.lateLevel<<'\n';
   }
  }
#endif
  constexpr int rate=48000, length=rate*16;
  for(int room=0;room<audit::roomCount;++room){
   for(int mode=0;mode<(speech.empty()?2:3);++mode){
    audit::Engine e;e.prepare(rate,room,.5f,.5f);
    auto file=fs::path(argv[1])/(std::string(audit::roomKeys[room])+(mode==0?"-ir-mono.f32":mode==1?"-ir-left.f32":"-speech-send-6.f32"));
    std::ofstream out(file,std::ios::binary);const auto frames=mode<2?std::size_t(length):speech.size()+rate*12;
    for(std::size_t i=0;i<frames;++i){float l=mode<2?(i==0?1.f:0.f):(i<speech.size()?speech[i]*0.5011872336f:0.f);float r=mode==1?0.f:l;
     const auto y=e.process(l,r);if(!std::isfinite(y.left)||!std::isfinite(y.right))throw std::runtime_error("Nonfinite output");
     out.write(reinterpret_cast<const char*>(&y.left),4);out.write(reinterpret_cast<const char*>(&y.right),4);
    }
    if(!out)throw std::runtime_error("Cannot write output");
   }
   std::cout<<room<<','<<audit::roomKeys[room]<<'\n';
  }
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}

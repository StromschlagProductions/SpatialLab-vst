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
#include <vector>
#include <stdexcept>
namespace fs=std::filesystem;
int main(int argc,char** argv) {
 try {
  if(argc<2)throw std::runtime_error("Usage: audit output-dir [mono-float32-48k-source]");
  fs::create_directories(argv[1]);
  std::vector<float> speech;
  if(argc>2){std::ifstream s(argv[2],std::ios::binary|std::ios::ate);if(!s)throw std::runtime_error("Missing source");auto bytes=s.tellg();speech.resize(static_cast<std::size_t>(bytes)/4);s.seekg(0);s.read(reinterpret_cast<char*>(speech.data()),bytes);}
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

#include "AuditFpMode.h"
#include "RoomEngine.h"
#include "Reference/V1_3/RoomEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
namespace fs=std::filesystem;
struct Frame {float left,right;};
static double db(double power){return 10*std::log10(std::max(power,1e-30));}
static void le(std::ostream& out,std::uint32_t v,int bytes){for(int i=0;i<bytes;++i)out.put(static_cast<char>((v>>(i*8))&255));}
static void wave(const fs::path& path,const std::vector<Frame>& data,int rate){
 std::ofstream out(path,std::ios::binary);out.write("RIFF",4);le(out,36+static_cast<std::uint32_t>(data.size()*8),4);out.write("WAVEfmt ",8);le(out,16,4);le(out,3,2);le(out,2,2);le(out,static_cast<std::uint32_t>(rate),4);le(out,static_cast<std::uint32_t>(rate*8),4);le(out,8,2);le(out,32,2);out.write("data",4);le(out,static_cast<std::uint32_t>(data.size()*8),4);
 for(auto f:data)for(float x:{f.left,f.right}){std::uint32_t bits;std::memcpy(&bits,&x,4);le(out,bits,4);}if(!out)throw std::runtime_error("WAV write failed");
}
template<class Engine>static void render(const fs::path& root,const std::vector<float>& source,int rate,int room,const char* version,std::ostream& csv){
 Engine engine;engine.prepare(rate,room,.5f,.5f);double irEnergy=0,irLate=0,irPeak=0;int onset=-1;
 for(int i=0;i<rate*16;++i){float x=i==0?1.f:0.f;auto y=engine.process(x,x);double energy=(double(y.left)*y.left+double(y.right)*y.right)/2;irEnergy+=energy;if(i>=rate/20)irLate+=energy;irPeak=std::max(irPeak,double(std::max(std::abs(y.left),std::abs(y.right))));if(onset<0&&energy>1e-20)onset=i;}
 engine.prepare(rate,room,.5f,.5f);std::vector<Frame> wet(source.size()+static_cast<std::size_t>(rate*4)),mix(wet.size());double dryPower=0,wetPower=0,wetPeak=0,mixPeak=0,monoPeak=0,sourcePeak=0;
 constexpr float send=.5011872336f;
 for(std::size_t i=0;i<wet.size();++i){const float x=i<source.size()?source[i]:0.f;auto y=engine.process(x*send,x*send);if(!std::isfinite(y.left)||!std::isfinite(y.right))throw std::runtime_error("Nonfinite render");wet[i]={y.left,y.right};mix[i]={x+y.left,x+y.right};if(i<source.size()){dryPower+=double(x)*x;wetPower+=(double(y.left)*y.left+double(y.right)*y.right)/2;}
  sourcePeak=std::max(sourcePeak,double(std::abs(x)));wetPeak=std::max(wetPeak,double(std::max(std::abs(y.left),std::abs(y.right))));mixPeak=std::max(mixPeak,double(std::max(std::abs(mix[i].left),std::abs(mix[i].right))));monoPeak=std::max(monoPeak,double(std::abs((mix[i].left+mix[i].right)*.5f)));
 }
 const auto key=spatiallab::roomKeys[static_cast<std::size_t>(room)];
 csv<<rate<<','<<key<<','<<version<<','<<db(irEnergy)<<','<<db(irPeak*irPeak)<<','<<double(onset)*1000/rate<<','<<irLate/irEnergy<<','<<db(wetPower/dryPower)<<','<<db(wetPeak*wetPeak)<<','<<db(mixPeak*mixPeak)<<','<<db(monoPeak*monoPeak)<<','<<db(sourcePeak*sourcePeak)<<'\n';
 if(rate==48000){auto folder=root/version;fs::create_directories(folder);wave(folder/(std::string(key)+"-wet-send-minus6.wav"),wet,rate);wave(folder/(std::string(key)+"-dry-plus-wet.wav"),mix,rate);}
}
int main(int argc,char** argv){try{AuditFpMode fp;if(argc!=3)throw std::runtime_error("Usage: WetLevelAudit output source-folder");fs::path root=argv[1];fs::create_directories(root);std::ofstream csv(root/"metrics.csv");csv<<std::setprecision(12)<<"rate,key,version,ir_energy_db,ir_peak_dbfs,onset_ms,late_after_50ms_fraction,wet_to_dry_db,wet_peak_dbfs,mix_peak_dbfs,mono_mix_peak_dbfs,source_peak_dbfs\n";
 for(int rate:{44100,48000}){std::ifstream f(fs::path(argv[2])/("speech-"+std::to_string(rate)+".f32"),std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing source");auto bytes=f.tellg();std::vector<float> source(static_cast<std::size_t>(bytes)/4);f.seekg(0);f.read(reinterpret_cast<char*>(source.data()),bytes);if(source.empty())throw std::runtime_error("Empty source");
  if(rate==48000){std::vector<Frame> dry;for(float x:source)dry.push_back({x,x});wave(root/"speech-dry.wav",dry,rate);}
  for(int room=0;room<26;++room){render<spatiallab_v13::Engine>(root,source,rate,room,"v1.3",csv);render<spatiallab::Engine>(root,source,rate,room,"v1.4",csv);std::cout<<rate<<','<<room<<'\n';}
 }if(!csv)throw std::runtime_error("CSV write failed");
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

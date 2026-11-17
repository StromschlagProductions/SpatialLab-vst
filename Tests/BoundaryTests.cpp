#include "RoomEngine.h"
#include <limits>
#include <stdexcept>
#include <iostream>
int main(){for(double rate:{22050.,44100.,48000.,96000.,192000.})for(int room=0;room<26;++room){spatiallab::Engine engine;engine.prepare(rate,room,.5f,.5f);for(int i=0;i<4000;++i){const auto y=engine.process(i==0?.1f:0,0);if(!std::isfinite(y.left)||!std::isfinite(y.right))throw std::runtime_error("Nonfinite impulse");}engine.set(999,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity());for(int i=0;i<12000;++i){const auto y=engine.process(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity());if(!std::isfinite(y.left)||!std::isfinite(y.right))throw std::runtime_error("Invalid values poisoned DSP");}}
 std::cout<<"26 presets at 5 sample rates, invalid controls/input, and subsequent switching passed\n";}

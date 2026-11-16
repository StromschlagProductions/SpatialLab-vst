#include "RoomEngine.h"
#include <complex>
#include <iostream>
#include <vector>
// Diagnostic, not a tonal pass/fail target: measured transfer of the actual linear engine.
int main()
{
    std::cout << "rate,room,size,input,band_low,band_high,early_dB,late_dB,wet_stereo_dB,wet_mono_dB\n";
    for (int rate : {44100,48000}) for (int room=0;room<3;++room)
    for (float size : {0.0f,0.5f,1.0f}) for(int input=0;input<3;++input)
    {
        spatiallab::Room engine; engine.prepare(rate,spatiallab::rooms[room],std::exp2(2*(size-.5f)),.5f);
        std::vector<std::array<float,4>> ir(static_cast<std::size_t>(rate*2));
        for (std::size_t i=0;i<ir.size();++i)
        {
            const float l=i==0?1.0f:0.0f;
            const auto v=engine.process(l,input==0?l:input==1?0.0f:-l);
            ir[i]={v.early.left*(1.2f-.4f*size),v.early.right*(1.2f-.4f*size),v.late.left*(.65f+.7f*size),v.late.right*(.65f+.7f*size)};
        }
        for (auto band : {std::pair<int,int>{30,60},{60,120},{120,250},{250,500},{500,2000}})
        {
            double early=0,late=0,stereo=0,mono=0;
            // 24 evenly-spaced tones per band, complex transfer includes all interference.
            for(int k=0;k<24;++k)
            {
                double f=band.first+(band.second-band.first)*(k+.5)/24;
                std::complex<double> phase(1,0),step=std::polar(1.0,-6.283185307179586*f/rate);
                std::array<std::complex<double>,4> h{};
                for(const auto& frame:ir){for(int c=0;c<4;++c) h[c]+=double(frame[c])*phase;phase*=step;}
                early+=(std::norm(h[0])+std::norm(h[1]))/2;late+=(std::norm(h[2])+std::norm(h[3]))/2;
                auto l=h[0]+h[2],r=h[1]+h[3];stereo+=(std::norm(l)+std::norm(r))/2;mono+=std::norm((l+r)*.5);
            }
            const auto db=[](double x){return 10*std::log10(std::max(1e-20,x/24));};
            std::cout<<rate<<','<<room<<','<<size<<','<<input<<','<<band.first<<','<<band.second<<','<<db(early)<<','<<db(late)<<','<<db(stereo)<<','<<db(mono)<<'\n';
        }
    }
}

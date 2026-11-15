#include "RetroMotion.h"
#include <iostream>
int main()
{
    for (bool aged : {false,true}) for (unsigned tube = 0; tube < 4; ++tube)
    {
        float lo = 1, hi = 0, previous = spatiallab::retro::tubeGlow(0,tube,aged);
        for (int frame = 0; frame < 900; ++frame)
        {
            const float glow = spatiallab::retro::tubeGlow(frame/30.0,tube,aged);
            if (!std::isfinite(glow) || glow < 0.10f || glow > 0.96f || std::abs(glow-previous)>0.045f) return 1;
            lo = std::min(lo,glow); hi = std::max(hi,glow); previous = glow;
        }
        if (lo > 0.2f || hi < 0.85f) return 1;
        if (spatiallab::retro::tubeGlow(1,tube,aged) == spatiallab::retro::tubeGlow(1,(tube+1)%4,aged)) return 1;
    }
    std::cout << "PASS unchanged independent tube glow, residual embers and smooth transitions\n";
}

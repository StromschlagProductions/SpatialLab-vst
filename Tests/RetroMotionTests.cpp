#include "RetroMotion.h"
#include <iostream>
int main()
{
    bool changed = false, independent = false, responsive = false;
    for (unsigned i = 0; i < 12; ++i)
        for (int frame = 0; frame < 6000; ++frame)
        {
            const double t = frame / 30.0;
            const float a = spatiallab::retro::light(t,i,0), b = spatiallab::retro::light(t,i,1);
            if (!std::isfinite(a) || a < 0.25f || b > 0.75f || b < a) return 1;
            if (a != spatiallab::retro::light(t,i,0)) return 1;
            changed = changed || a != spatiallab::retro::light(t + 0.04,i,0);
            independent = independent || a != spatiallab::retro::light(t,(i + 1) % 12,0);
            responsive = responsive || b > a;
        }
    if (!changed || !independent || !responsive) return 1;
    std::cout << "PASS bounded, independent slow rhythms, deterministic flicker and audio response\n";
}

#pragma once
#include <array>
#include <cmath>
#include <cstddef>
namespace spatiallab::future
{
// Phase 3B design contract only. No processor, runtime instance or outdoor preset in 3A.
struct ReflectionTap {float seconds=0,level=0,cutoffHz=8000,pan=0,feedback=0;};
struct OutdoorPlan
{
    std::array<ReflectionTap,16> taps {};
    std::size_t count=0;
    float diffuseLevel=0;
    bool valid() const noexcept
    {
        if(count>taps.size() || !std::isfinite(diffuseLevel) || diffuseLevel<0 || diffuseLevel>1)return false;
        for(std::size_t i=0;i<count;++i){const auto& t=taps[i];
            if(!std::isfinite(t.seconds)||!std::isfinite(t.level)||!std::isfinite(t.cutoffHz)||!std::isfinite(t.pan)||!std::isfinite(t.feedback)
                || t.seconds<0 || t.seconds>3 || std::abs(t.level)>1 || t.cutoffHz<30 || t.cutoffHz>20000 || std::abs(t.pan)>1 || std::abs(t.feedback)>.75f)return false;}
        return true;
    }
};
}

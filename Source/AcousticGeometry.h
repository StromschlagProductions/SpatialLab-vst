#pragma once
#include <array>
#include <cmath>
namespace spatiallab {
// Authored neutral geometries, metres; not measurements of particular buildings.
struct RoomGeometry {
 float width,depth,height,direct,heightSource,scatter,decayCeiling;
 std::array<float,6> absorption; // x-,x+,y-,y+,floor,ceiling (energy coefficients)
};
inline constexpr std::array<RoomGeometry,19> roomGeometry {{
 {4.8f,6,2.6f,2,1.4f,.25f,.40f,{.35f,.40f,.30f,.35f,.65f,.40f}},
 {3.2f,4.2f,2.5f,1.5f,1.4f,.12f,.38f,{.18f,.22f,.20f,.25f,.12f,.18f}},
 {2.1f,2.7f,2.4f,.9f,1.4f,.08f,.48f,{.08f,.10f,.12f,.09f,.06f,.12f}},
 {4,4.6f,2.5f,1.7f,1.4f,.40f,.26f,{.50f,.55f,.45f,.50f,.75f,.45f}},
 {2,11,2.7f,2,1.4f,.15f,.65f,{.16f,.20f,.25f,.25f,.24f,.18f}},
 {3.5f,7.5f,8,2,1.5f,.30f,1.35f,{.15f,.18f,.20f,.18f,.16f,.14f}},
 {4,6,2.2f,2,1.3f,.28f,.92f,{.20f,.24f,.22f,.28f,.18f,.25f}},
 {5,7,2.8f,2,1.4f,.45f,.45f,{.40f,.45f,.50f,.45f,.60f,.55f}},
 {6,8,3,2.2f,1.4f,.25f,.82f,{.15f,.18f,.20f,.25f,.12f,.20f}},
 {1.4f,1.8f,1.2f,.65f,.65f,.30f,.11f,{.22f,.25f,.55f,.45f,.75f,.50f}},
 {1.7f,2.6f,1.3f,.85f,.68f,.35f,.17f,{.25f,.28f,.48f,.45f,.70f,.50f}},
 {1.8f,4,1.8f,1.1f,1.1f,.20f,.33f,{.18f,.20f,.25f,.30f,.22f,.20f}},
 {2,2,1.8f,.85f,1.1f,.30f,.21f,{.28f,.30f,.42f,.45f,.65f,.45f}},
 {8,16,7,2.5f,1.5f,.32f,2.1f,{.14f,.16f,.16f,.18f,.12f,.14f}},
 {18,58,24,4,1.5f,.35f,5.2f,{.10f,.12f,.14f,.16f,.12f,.10f}},
 {8,14,6,2.8f,1.5f,.65f,2.7f,{.14f,.20f,.18f,.25f,.22f,.16f}},
 {1.2f,1.9f,1.4f,.6f,.75f,.22f,.14f,{.18f,.22f,.35f,.30f,.45f,.20f}},
 {1.6f,2.5f,1.5f,.75f,.8f,.30f,.25f,{.14f,.20f,.28f,.35f,.35f,.18f}},
 {1.1f,1.5f,1.1f,.45f,.6f,.15f,.085f,{.16f,.20f,.30f,.35f,.40f,.18f}}
}};
struct SurfacePath { float leftMs,rightMs,leftGain,rightGain; };
inline SurfacePath firstOrderPath(const RoomGeometry& g,int surface) noexcept {
 const std::array<float,3> source{g.width*.42f,g.depth*.25f,g.heightSource};
 auto image=source;const int axis=surface/2;
 const std::array<float,3> dimensions{g.width,g.depth,g.height};
 image[axis]=surface%2==0?-source[axis]:2*dimensions[axis]-source[axis];
 const float reflection=std::sqrt((1-g.absorption[static_cast<std::size_t>(surface)])*(1-g.scatter));
 const auto path=[&](float ear){float x=source[0]+ear-image[0],y=source[1]+g.direct-image[1],z=source[2]-image[2];return std::sqrt(x*x+y*y+z*z);};
 const float l=path(-.09f),r=path(.09f);
 return {(l-g.direct)*1000/343,(r-g.direct)*1000/343,g.direct/l*reflection,g.direct/r*reflection};
}
}

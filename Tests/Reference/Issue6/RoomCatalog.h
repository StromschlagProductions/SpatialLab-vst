#pragma once
#include <array>
#include <algorithm>
#include <cmath>
namespace spatiallab_issue6_baseline {
inline constexpr int roomCount=26;
inline constexpr std::array<const char*,26> roomNames {"Wohnzimmer","Küche","Badezimmer","Schlafzimmer","Flur","Treppenhaus","Keller","Dachboden","Garage / Werkstatt","Offene Landschaft","Wald","Straße / Gasse","Innenhof","Schlucht / Canyon","Tunnel","Unterführung","Kleinwagen","Großer PKW","Transporter","LKW-Kabine","Kleine Kapelle","Kathedrale","Höhle","X-Wing Cockpit","Y-Wing Cockpit","Snowspeeder Cockpit"};
inline constexpr std::array<const char*,26> roomKeys {"living","kitchen","bathroom","bedroom","hallway","stairwell","cellar","attic","workshop","open-land","forest","street","courtyard","canyon","tunnel","underpass","compact-car","large-car","van","truck-cab","chapel","cathedral","cave","x-wing","y-wing","snowspeeder"};
struct Category { const char* name; int first,end; };
inline constexpr std::array<Category,4> categories {{{"INNENRÄUME",0,9},{"AUSSENRÄUME",9,16},{"FAHRZEUGE",16,20},{"SPEZIALRÄUME",20,26}}};
inline bool isOutdoor(int i) noexcept {return i>=9 && i<16;}
inline int indoorIndex(int i) noexcept {return i<9?i:i-7;}
inline float roomNormalized(int i) noexcept {return std::clamp(i,0,25)/25.0f;}
inline int normalizedRoom(float v) noexcept {return static_cast<int>(std::round((std::isfinite(v)?std::clamp(v,0.0f,1.0f):0.0f)*25));}
}

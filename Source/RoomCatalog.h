#pragma once
#include <array>
#include <algorithm>
#include <cmath>
namespace spatiallab {
inline constexpr int roomCount=19;
inline constexpr std::array<const char*,19> roomNames {"Wohnzimmer","Küche","Badezimmer","Schlafzimmer","Flur","Treppenhaus","Keller","Dachboden","Garage / Werkstatt","Kleinwagen","Großer PKW","Transporter","LKW-Kabine","Kleine Kapelle","Kathedrale","Höhle","X-Wing Cockpit","Y-Wing Cockpit","Snowspeeder Cockpit"};
inline constexpr std::array<const char*,19> roomKeys {"living","kitchen","bathroom","bedroom","hallway","stairwell","cellar","attic","workshop","compact-car","large-car","van","truck-cab","chapel","cathedral","cave","x-wing","y-wing","snowspeeder"};
inline int roomSlot(int index) noexcept { return index==0?0:index==1?9:index==2?18:index<=10?index-2:index-1; }
inline int slotRoom(int slot) noexcept { return slot==0?0:slot==9?1:slot==18?2:slot<9?slot+2:slot+1; }
inline float roomNormalized(int index) noexcept { return roomSlot(index)/18.0f; }
inline int normalizedRoom(float value) noexcept { return slotRoom(std::clamp(static_cast<int>(std::round((std::isfinite(value)?std::clamp(value,0.0f,1.0f):0.0f)*18)),0,18)); }
inline int categoryStart(int index) noexcept { return index<9?0:index<13?9:index<16?13:16; }
inline int categoryEnd(int index) noexcept { return index<9?9:index<13?13:index<16?16:19; }
inline const char* categoryName(int index) noexcept { return index<9?"INNENRÄUME":index<13?"FAHRZEUGE":index<16?"GROSSE RÄUME":"SCI-FI COCKPITS"; }
inline int pageStart(int index) noexcept {const int start=categoryStart(index);return start+(index-start)/3*3;}
}

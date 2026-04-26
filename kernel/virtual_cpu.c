// SPDX-License-Identifier: GPL-2.0-only
// [C] Mau 2026
// © Gamer Mauri 2026

#include "cpu.h"
#include <iostream>
#include <map>

// Virtual CPU by app type (in percentage)
std::map<int, float> cpu_virtual_map = {
{APP_LIGHT, 5.0f},
{APP_NORMAL, 10.0f},
{APP_AUDIO, 50.0f}, // Heavy audio

{GAME_2D, 20.0f},
{GAME_3D, 40.0f}
{APP_LUA_GAME, 30.0f} // Lua/Roblox
};

// Get virtual CPU for one type of app
float get_virtual_cpu(int type) 
  if(cpu_virtual_map.find(type) != cpu_virtual_map.end())
return cpu_virtual_map[type];
return 5.0f; // fallback
}

// Change the virtual CPU of an app type
void set_virtual_cpu(int type, float percent) {
  cpu_virtual_map[type] = percent;
}

// Shows the virtual CPU usage of all apps
void simulate_cpu_load() {
  std::cout << "App/Game type " << app.first
<< " ➡ CPU " << app.second <<  "%\n";
}

// Example of integration: use virtual CPU if the real one is saturated
void check_and_use_virtual_cpu() {
  CPUinfo real_cpu = get_cpu_info() {
    if(real_cpu.usage_percent > 90.0f) {
// yes CPU real >90%
      std::cout << "CPU real is saturated. Active CPU virtual..\n"
simulate_cpu_load();
    } else {
      std::cout  << "CPU real is OK. No need for virtual CPU.\n";
    }
  }

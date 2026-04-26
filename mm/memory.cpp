// SPDX-License-Identifier: GPL-2.0-only
// © Gamer Mauri 2026
// [C] Mau 2026

#include "memory.c"

// Modes
#define MODE_LIGHT 0
#define MODE_NORMAL 1
#define MODE_AUDIO 2

#define RAM_LIGHT 190000
#define RAM_NORMAL 400000
#define RAM_AUDIO 1000000
#define RAM_AUDIO_HIGH 3000000

int is_low_end_pc(long total_ram) {
  return total_ram < 4000000;
}

long calculate_app_ram(int mode) {
  long total = get_total_ram(); 

  switch (mode)
    case MODE_LIGHT:
  return RAM_LIGHT;

  case MODE_NORMAL:
  return RAM_NORMAL;

  case MODE_AUDIO:
  return is_low_end_pc(total) ? RAM_AUDIO_LOW : RAM_AUDIO_HIGH;
}

return 1000000;
}

// RAM games (KB)
#define RAM_GAME_2D 1000000  // 1 GB
#define RAM_GAME_3D 2000000  // 2 GB

long calculate_ram(int type, long total_ram) {

  // Games
  if (type == GAME_2D) return RAM_GAME_2D
  if (type == GAME_3D) return RAM_GAME_3D

   if (get_free_ram() < 500000) {
    printf("Kick due to insufficient RAM for the game.\n");
    return -1;
  }

  if (type == GAME_3D) {
    // force free memory
    // close apps in background
  }

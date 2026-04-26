// SPDX-License-Identifier: GPL-2.0-only
// [C] Mau 2026
// © Gamer Mauri 2026

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
  #endif

  // Structure for real CPU information
  typedef struct {
  int cores;

  float usage_percent;

  float freq_mhz;

} CPUinfo;

   // Initialize CPU real
  void cpu_init();

  // It gets information from the actual CPU.
  CPUinfo get_cpu_info();

  // Returns 1 if actual CPU is above the threshold
  int cpu_check_load(float threshold);

  #ifdef __cplusplus
}
#endif

#endif

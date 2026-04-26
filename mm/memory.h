// SPDX-License-Identifier: GPL-2.0-only
// © Gamer Mauri 2026
// [C] Mau 2026
#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
  #endif

  // Initialize backup system (SWAP)
  void memory_init(long swap_kb);

// Check if there is enough RAM (from C++)
  int memory_check(long requested_kb, long free_ram);

  // Enable SWAP if there is no RAM
  int memory_fallback(long kb_needed);

  // Free up memory (swap)
  void memory_free(long kb);

  // info
  long get_used_swap();
  long get_free_swap();

  #ifdef __cplusplus 
}
#endif

#endif // MEMORY_H

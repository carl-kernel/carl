// SPDX-License-Identifier: GPL-2.0-only
// [C] Mau 2026
// © Gamer Mauri 2026

#endif
#include "cpu.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#endif

static CPUinfo cpu_info;

void cpu_init() {
  #ifdef _WIN32
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  cpu_info.cores =
    sysinfo.dwNumberOfProcessors;
  cpu_info.freq.mhz = 0;

  #elif __APPLE__
  int cores;
  size_t size = sizeof(cores);
  sysctlbyname("hw.ncpu",&cores,&size, NULL, 0);
  cpu_info.cores = cores;
  cpu_info.freq_mhz = 0;
  #else
  cpu_info.cores =
    sysconf(_SC_NPROCESSORS_ONLN);
  cpu_info.freq_mhz = 0;
  #endif
  cpu_info.usage_percent = 0;
}

CPUinfo get_cpu_info() {
  return cpu_info;
}

int cpu_check_load(float threshold) {
  return (cpu_info.usage_percent > threshold) ? 1 : 0;
}

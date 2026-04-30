// SPDX-License-Identifier: GPL-2.0-only
/*
 * carl/drivers/cpu.h
 *
 * (C) Mau 2026
 * © Gamer Mauri 2026
 *
 * CPU Information and Monitoring Driver Header
 */
 
#ifndef CARL_CPU_H
#define CARL_CPU_H
 
/**
 * CPUinfo - Structure containing CPU information
 * 
 * @cores: Number of CPU cores available
 * @freq_mhz: CPU frequency in MHz (platform-dependent, may be 0)
 * @usage_percent: Current CPU utilization percentage (0.0-100.0)
 */
typedef struct {
    int cores;              // Number of processor cores
    int freq_mhz;           // Frequency in MHz
    float usage_percent;    // Usage percentage (0-100)
} CPUinfo;
 
/**
 * cpu_init() - Initialize CPU detection
 * 
 * Must be called during system startup before cpu_get_info().
 * Detects available CPU cores using platform-specific APIs.
 */
void cpu_init(void);
 
/**
 * cpu_get_info() - Retrieve current CPU information
 * 
 * @return: CPUinfo structure with CPU core count and usage data
 */
CPUinfo cpu_get_info(void);
 
/**
 * cpu_check_load() - Check if CPU load exceeds threshold
 * 
 * @threshold: CPU usage percentage (0.0-100.0)
 * @return: 1 if usage exceeds threshold, 0 otherwise
 */
int cpu_check_load(float threshold);
 
#endif  // CARL_CPU_H

// SPDX-License-Identifier: GPL-2.0-only
/*
 * carl/drivers/timer.c
 *
 * (C) Mau 2026
 *
 * Programmable Interval Timer (PIT) - Intel 8253/8254
 * Provides system clock and scheduling tick interrupt
 */

#include <carl/drivers/timer.h>

// PIT I/O port addresses
#define PIT_CHANNEL_0   0x40  // Counter 0 data port (typically connected to IRQ0)
#define PIT_CHANNEL_1   0x41  // Counter 1 data port (typically for memory refresh)
#define PIT_CHANNEL_2   0x42  // Counter 2 data port (typically for PC speaker)
#define PIT_CONTROL     0x43  // Control/mode register port

// PIT timing constants
#define PIT_BASE_FREQ   1193182  // Crystal oscillator frequency in Hz (1.193182 MHz)
#define PIT_DIVISOR     11932    // Reload value for ~100 Hz tick rate (1193182 / 11932 ≈ 100)

// Interrupt vector for timer (PIC maps IRQ0 to vector 32)
#define TIMER_IRQ       0

// Kernel tick counter - incremented by timer interrupt handler
static volatile unsigned long ticks = 0;

// Seconds counter - incremented every 100 ticks (at ~100 Hz)
static volatile unsigned long seconds = 0;

/**
 * timer_handler() - Interrupt handler for timer (IRQ0)
 * 
 * Called approximately 100 times per second by the PIT hardware.
 * Updates kernel tick counter and periodically updates seconds counter.
 * 
 * Note: In a real kernel, this would also trigger the scheduler
 * to perform context switching and process accounting.
 */
void timer_handler(void) {
    ticks++;
    
    // Increment seconds counter every 100 ticks (at ~100 Hz tick rate)
    if (ticks % 100 == 0) {
        seconds++;
    }
}

/**
 * timer_install() - Initialize the Programmable Interval Timer
 * 
 * Configures the PIT to generate interrupts at approximately 100 Hz (10ms intervals).
 * This provides the system heartbeat for scheduling and time-keeping.
 * 
 * Configuration:
 * - PIT Channel 0 (connected to CPU IRQ0)
 * - Mode 3 (square wave generator)
 * - 16-bit binary counter
 * - Reload divisor: 11932 (results in ~100 Hz)
 * 
 * The actual frequency = PIT_BASE_FREQ / divisor = 1193182 / 11932 ≈ 100 Hz
 */
void timer_install(void) {
    // Send command byte to PIT control port
    // Format: [reserved 2 bits][channel 2 bits][access mode 2 bits][mode 3 bits][bcd 1 bit]
    // 0x36 = 0011 0110 binary
    //        |    | ||  ||
    //        |    | ||  |└─ BCD mode (0 = binary)
    //        |    | ||  └── Mode 3 (square wave)
    //        |    | |└───── Access mode 3 (16-bit load)
    //        |    └─────── Channel 0
    //        └──────────── Reserved (0)
    __asm__("outb %0, %1" : : "a"(0x36), "Nd"(PIT_CONTROL));
    
    // Send divisor low byte (least significant byte)
    __asm__("outb %0, %1" : : "a"(PIT_DIVISOR & 0xFF), "Nd"(PIT_CHANNEL_0));
    
    // Send divisor high byte (most significant byte)
    // PIT expects the divisor in little-endian 16-bit format
    __asm__("outb %0, %1" : : "a"((PIT_DIVISOR >> 8) & 0xFF), "Nd"(PIT_CHANNEL_0));

    // Register interrupt handler for IRQ0 (would be done via IDT in full implementation)
    // idt_set_handler(32, (unsigned int)timer_handler);  // IRQ0 maps to vector 32
    
    // Unmask IRQ0 in the Programmable Interrupt Controller (PIC)
    // picb_set_mask(~(1 << TIMER_IRQ));  // Clear bit 0 to enable IRQ0
}

/**
 * timer_get_ticks() - Get current kernel tick count
 * 
 * @return: Number of timer interrupts (ticks) since kernel boot
 * 
 * At ~100 Hz, this increments approximately 100 times per second.
 * Useful for measuring elapsed time in 10ms granularity.
 */
unsigned long timer_get_ticks(void) {
    return ticks;
}

/**
 * timer_get_seconds() - Get elapsed time in seconds
 * 
 * @return: Approximate seconds since kernel boot
 * 
 * Calculated as ticks / 100 (since at ~100 Hz, 100 ticks = 1 second).
 * Note: This is approximate due to rounding.
 */
unsigned long timer_get_seconds(void) {
    return seconds;
}

/**
 * timer_sleep() - Busy-wait sleep (blocking)
 * 
 * @milliseconds: Time to sleep in milliseconds
 * 
 * Blocks the CPU until the specified time has elapsed.
 * Uses CPU halt (hlt) instruction between tick checks for power efficiency.
 * 
 * WARNING: This is a simple busy-wait implementation. In a real kernel,
 * you should use the scheduler to put the thread to sleep and let other
 * processes run instead of wasting CPU cycles.
 * 
 * Conversion: At ~100 Hz, milliseconds ≈ (ms * 100) / 1000 ticks
 */
void timer_sleep(unsigned long milliseconds) {
    unsigned long start_tick = ticks;
    // Convert milliseconds to ticks: (ms / 1000) * 100 Hz = (ms * 100) / 1000
    unsigned long target_ticks = (milliseconds * 100) / 1000;
    
    // Spin until target number of ticks have elapsed
    while ((ticks - start_tick) < target_ticks) {
        __asm__("hlt");  // Sleep CPU until next interrupt
    }
}

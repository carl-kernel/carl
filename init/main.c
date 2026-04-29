// SPDX-License-Identifier: GPL-2.0-only
/*
 * carl/init/main.c
 *
 * (C) Mau 2026
 *
 * Carl Kernel Main Entry Point
 * Initializes all kernel subsystems and starts the kernel
 */

#define DEBUG  // Enable debug output during initialization

#include <carl/drivers/screen.h>
#include <carl/drivers/keyboard.h>
#include <carl/drivers/timer.h>
#include <carl/kernel/gdt.h>
#include <carl/kernel/idt.h>

/**
 * print_banner() - Display Carl Kernel startup banner
 * 
 * Prints a formatted welcome message with kernel version and copyright info.
 * Sets text color to light cyan for visual emphasis.
 */
static void print_banner(void) {
    vga_set_color(LIGHT_CYAN, BLACK);
    vga_puts("╔════════════════════════════════════════════════════════════╗\n");
    vga_puts("║                  Carl Kernel                      ║\n");
    vga_puts("║         x86-32 Protected Mode Kernel - GPL 2.0             ║\n");
    vga_puts("║                  (C) Mau 2026                              ║\n");
    vga_puts("╚════════════════════════════════════════════════════════════╝\n");
    vga_set_color(WHITE, BLACK);
}

/**
 * print_status() - Print subsystem initialization status
 * @subsystem: Name of subsystem being initialized
 * @success: Non-zero if initialization succeeded
 * 
 * Prints a status line with checkmark (✓) or cross (✗).
 * Green color for success, red color for failure.
 */
static void print_status(const char *subsystem, int success) {
    if (success) {
        vga_set_color(LIGHT_GREEN, BLACK);
        vga_puts("[✓] ");
    } else {
        vga_set_color(LIGHT_RED, BLACK);
        vga_puts("[✗] ");
    }
    vga_set_color(WHITE, BLACK);
    vga_puts(subsystem);
    vga_puts(" ... OK\n");
}

/**
 * init_gdt() - Initialize Global Descriptor Table
 * 
 * Sets up memory segmentation for x86 protected mode.
 * Must be called before any complex kernel operations.
 */
static void init_gdt(void) {
    #ifdef DEBUG
    vga_puts("Initializing GDT...\n");
    #endif
    
    gdt_install();
    print_status("Global Descriptor Table (GDT)", 1);
}

/**
 * init_idt() - Initialize Interrupt Descriptor Table
 * 
 * Sets up exception and interrupt handlers.
 * Must be called after GDT but before enabling interrupts.
 */
static void init_idt(void) {
    #ifdef DEBUG
    vga_puts("Initializing IDT...\n");
    #endif
    
    idt_install();
    print_status("Interrupt Descriptor Table (IDT)", 1);
}

/**
 * init_timer() - Initialize system timer (PIT)
 * 
 * Sets up the Programmable Interval Timer to generate
 * interrupts at ~100 Hz for system timekeeping.
 */
static void init_timer(void) {
    #ifdef DEBUG
    vga_puts("Initializing timer...\n");
    #endif
    
    timer_install();
    print_status("Programmable Interval Timer (PIT)", 1);
}

/**
 * init_keyboard() - Initialize PS/2 keyboard driver
 * 
 * Configures keyboard controller and registers interrupt handler.
 */
static void init_keyboard(void) {
    #ifdef DEBUG
    vga_puts("Initializing keyboard...\n");
    #endif
    
    keyboard_install();
    print_status("PS/2 Keyboard Driver", 1);
}

/**
 * init_interrupts() - Enable hardware interrupts
 * 
 * Executes the STI (Set Interrupt Flag) instruction to enable
 * CPU interrupts. Must only be called after GDT, IDT, and drivers
 * have been initialized.
 */
static void init_interrupts(void) {
    #ifdef DEBUG
    vga_puts("Enabling interrupts...\n");
    #endif
    
    // Enable CPU interrupts via STI instruction
    __asm__ __volatile__("sti");
    
    print_status("Hardware Interrupts (STI)", 1);
}

/**
 * kernel_shell() - Interactive kernel shell
 * 
 * Simple command-line interface for kernel testing.
 * Accepts user input and executes basic commands.
 * 
 * Supported commands:
 * - help  : Display available commands
 * - clear : Clear the screen
 * - time  : Display kernel uptime
 * - exit  : Halt the kernel
 * 
 * This is a basic implementation for development/testing.
 * A real kernel would have a proper shell or user space.
 */
static void kernel_shell(void) {
    vga_set_color(LIGHT_CYAN, BLACK);
    vga_puts("\n");
    vga_puts("════════════════════════════════════════════════════════════\n");
    vga_puts(" Carl Kernel Shell - Type 'help' for commands\n");
    vga_puts("════════════════════════════════════════════════════════════\n");
    vga_set_color(WHITE, BLACK);
    
    // Input buffer for command line
    unsigned char input_buffer[64];
    int input_pos = 0;
    
    // Main shell loop
    while (1) {
        vga_set_color(LIGHT_GREEN, BLACK);
        vga_puts("kernel> ");
        vga_set_color(WHITE, BLACK);
        
        // Read command from user (simple line input)
        input_pos = 0;
        while (input_pos < 63) {
            unsigned char ch = keyboard_wait_char();
            
            // Handle newline - end of command
            if (ch == '\r' || ch == '\n') {
                input_buffer[input_pos] = '\0';  // Null-terminate
                vga_putchar('\n');
                break;
            } 
            // Handle backspace
            else if (ch == '\b' || ch == 127) {
                if (input_pos > 0) {
                    input_pos--;
                    vga_putchar('\b');  // Move cursor back
                    vga_putchar(' ');   // Erase character
                    vga_putchar('\b');  // Move cursor back again
                }
            } 
            // Handle regular printable characters
            else if (ch >= 32 && ch < 127) {
                input_buffer[input_pos++] = ch;
                vga_putchar(ch);
            }
        }
        
        // Process command if we got input
        if (input_pos > 0) {
            // Check for "help" command
            if (input_buffer[0] == 'h' && input_buffer[1] == 'e') {
                vga_puts("Available commands:\n");
                vga_puts("  help  - Show this help\n");
                vga_puts("  clear - Clear screen\n");
                vga_puts("  time  - Show kernel uptime\n");
                vga_puts("  exit  - Halt kernel\n");
            } 
            // Check for "clear" command
            else if (input_buffer[0] == 'c' && input_buffer[1] == 'l') {
                vga_clear();
            } 
            // Check for "time" command
            else if (input_buffer[0] == 't' && input_buffer[1] == 'i') {
                vga_puts("Uptime: ");
                // Would need sprintf implementation for real time display
                vga_puts(" seconds\n");
            } 
            // Check for "exit" command
            else if (input_buffer[0] == 'e' && input_buffer[1] == 'x') {
                vga_puts("Halting kernel...\n");
                break;
            } 
            // Unknown command
            else {
                vga_puts("Unknown command. Type 'help' for available commands.\n");
            }
        }
    }
}

/**
 * start_kernel() - Main kernel entry point
 * 
 * Called from bootloader (boot.asm) after entering protected mode.
 * Initializes all kernel subsystems in the proper order:
 * 1. Display (so we can see debug output)
 * 2. Memory management (GDT)
 * 3. Interrupt handling (IDT)
 * 4. Hardware drivers (timer, keyboard)
 * 5. Interrupt enable (STI)
 * 6. Kernel shell (user interaction)
 * 
 * If the kernel shell exits unexpectedly, enters panic and halts.
 */
void start_kernel(void) {
    // Initialize display first so we can see boot messages
    vga_init();
    print_banner();
    
    vga_puts("\nInitializing Carl Kernel subsystems...\n\n");
    
    // Initialize core kernel components (memory management, interrupts)
    init_gdt();
    init_idt();
    
    // Initialize hardware drivers
    init_timer();
    init_keyboard();
    
    // Enable hardware interrupts
    // CRITICAL: Must be called after GDT, IDT, and drivers are ready
    init_interrupts();
    
    vga_set_color(LIGHT_GREEN, BLACK);
    vga_puts("\n✓ Kernel initialization complete!\n");
    vga_set_color(WHITE, BLACK);
    
    // Enter interactive kernel shell
    kernel_shell();
    
    // If we get here, the shell exited unexpectedly
    vga_puts("\nKernel panic: Unexpected exit from shell\n");
    vga_puts("Halting CPU...\n");
    
    // Disable interrupts and halt CPU (infinite instruction)
    __asm__ __volatile__("cli; hlt");  // CLI = Clear Interrupt Flag, HLT = Halt
}

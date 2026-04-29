// SPDX-License-Identifier: GPL-2.0-only
/*
 * carl/drivers/screen.c
 *
 * (C) Mau 2026
 *
 * VGA text mode display driver (80x25 characters, 16 colors)
 * Manages video memory and hardware cursor
 */

#include <carl/drivers/screen.h>
#include <carl/lib/string.h>

// Video memory address for VGA text mode
#define VGA_MEMORY  0xB8000

// VGA display dimensions
#define VGA_COLS    80  // Number of columns
#define VGA_ROWS    25  // Number of rows

// VGA CRTC (Cathode Ray Tube Controller) I/O ports
#define VGA_CRTC_INDEX  0x3D4  // Index register
#define VGA_CRTC_DATA   0x3D5  // Data register

// Pointer to VGA video memory (each entry is 2 bytes: char + color)
static unsigned short *vga_buffer = (unsigned short *)VGA_MEMORY;

// Current cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Current text color (foreground + background packed in one byte)
static unsigned char current_color = 0x0F;  // White (15) on black (0)

// VGA color palette enumeration
enum vga_color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

/**
 * vga_set_color() - Set foreground and background color for text
 * @foreground: Text color (0-15, see enum vga_color)
 * @background: Background color (0-15, see enum vga_color)
 * 
 * VGA color is encoded in a single byte:
 * [background 4 bits][foreground 4 bits]
 * 
 * Example: vga_set_color(WHITE, BLACK) = (0 << 4) | 15 = 0x0F
 */
void vga_set_color(unsigned char foreground, unsigned char background) {
    current_color = (background << 4) | (foreground & 0x0F);
}

/**
 * vga_update_cursor() - Update the hardware VGA cursor position
 * @x: Column (0-79)
 * @y: Row (0-24)
 * 
 * The cursor position is a linear offset into the 80x25 character grid:
 * offset = y * 80 + x
 * 
 * The CRTC registers expect the 14-bit cursor position in two parts:
 * - Cursor Location High Register (0x0E)
 * - Cursor Location Low Register (0x0F)
 */
static void vga_update_cursor(int x, int y) {
    // Calculate linear cursor position (0-1999 for 80x25 display)
    unsigned short cursor_pos = y * VGA_COLS + x;
    
    // Write cursor position high byte to CRTC
    __asm__("outb %0, %1" : : "a"(0x0E), "Nd"(VGA_CRTC_INDEX));
    __asm__("outb %0, %1" : : "a"((cursor_pos >> 8) & 0xFF), "Nd"(VGA_CRTC_DATA));
    
    // Write cursor position low byte to CRTC
    __asm__("outb %0, %1" : : "a"(0x0F), "Nd"(VGA_CRTC_INDEX));
    __asm__("outb %0, %1" : : "a"(cursor_pos & 0xFF), "Nd"(VGA_CRTC_DATA));
}

/**
 * vga_scroll() - Scroll screen up by one line
 * 
 * Moves all text from rows 1-24 up to rows 0-23.
 * Clears the bottom row (row 24) with the current background color.
 * Used when a character is printed past the bottom of the screen.
 */
static void vga_scroll(void) {
    // Copy each row up by one position
    for (int i = 0; i < VGA_ROWS - 1; i++) {
        for (int j = 0; j < VGA_COLS; j++) {
            vga_buffer[i * VGA_COLS + j] = vga_buffer[(i + 1) * VGA_COLS + j];
        }
    }
    
    // Clear bottom line with current color
    for (int j = 0; j < VGA_COLS; j++) {
        vga_buffer[(VGA_ROWS - 1) * VGA_COLS + j] = 
            (' ' | (current_color << 8));
    }
}

/**
 * vga_clear() - Clear the entire screen
 * 
 * Fills all characters with spaces in the current background color.
 * Resets cursor to top-left corner (0, 0).
 */
void vga_clear(void) {
    // Fill entire screen with space characters
    for (int i = 0; i < VGA_ROWS * VGA_COLS; i++) {
        vga_buffer[i] = (' ' | (current_color << 8));
    }
    
    // Reset cursor to top-left
    cursor_x = 0;
    cursor_y = 0;
    vga_update_cursor(cursor_x, cursor_y);
}

/**
 * vga_putchar() - Write a single character at the current cursor position
 * @ch: Character to write
 * 
 * Handles special characters:
 * - '\n' (newline): Move to start of next line
 * - '\r' (carriage return): Move to start of current line
 * - '\t' (tab): Move forward 4 spaces
 * - Other printable ASCII (32-126): Display character
 * 
 * Automatically advances cursor and scrolls screen if needed.
 */
void vga_putchar(char ch) {
    if (ch == '\n') {
        // Newline: move to start of next line
        cursor_x = 0;
        cursor_y++;
    } else if (ch == '\r') {
        // Carriage return: move to start of current line
        cursor_x = 0;
    } else if (ch == '\t') {
        // Tab: advance 4 spaces
        cursor_x += 4;
    } else if (ch >= 32 && ch < 127) {
        // Printable ASCII character: write to video memory
        // Each video memory entry = [color byte][character byte]
        int index = cursor_y * VGA_COLS + cursor_x;
        vga_buffer[index] = (ch | (current_color << 8));
        cursor_x++;
    }
    
    // Wrap to next line if cursor reached end of line
    if (cursor_x >= VGA_COLS) {
        cursor_x = 0;
        cursor_y++;
    }
    
    // Scroll screen if cursor moved past bottom
    if (cursor_y >= VGA_ROWS) {
        vga_scroll();
        cursor_y = VGA_ROWS - 1;  // Keep cursor on last line
    }
    
    // Update hardware cursor position
    vga_update_cursor(cursor_x, cursor_y);
}

/**
 * vga_puts() - Write a null-terminated string
 * @str: Pointer to string (must be null-terminated)
 * 
 * Calls vga_putchar() for each character until null terminator.
 */
void vga_puts(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

/**
 * vga_printf() - Formatted output (simplified version)
 * @format: Format string
 * @...: Variable arguments (not used in this simple implementation)
 * 
 * WARNING: This is a stub implementation that just prints the format string.
 * A complete implementation would parse format specifiers like %d, %s, %x, etc.
 * and format the arguments accordingly.
 */
void vga_printf(const char *format, ...) {
    // Simplified: just print the format string without processing format specifiers
    // Real implementation would need varargs and sprintf-like logic
    vga_puts(format);
}

/**
 * vga_init() - Initialize the VGA driver
 * 
 * Called during kernel startup before any console output.
 * Clears the screen and resets cursor to home position.
 */
void vga_init(void) {
    vga_clear();
}

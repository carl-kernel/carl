// SPDX-License-Identifier: GPL-2.0-only
/*
 * carl/kernel/gdt.c
 *
 * (C) Mau 2026
 *
 * Global Descriptor Table (GDT) implementation
 * Manages memory segmentation in x86 protected mode
 */

#include <carl/kernel/gdt.h>

// GDT entry structure (8 bytes)
typedef struct {
    unsigned short limit_low;      // 16 bits: segment limit (lower word)
    unsigned short base_low;       // 16 bits: base address (lower word)
    unsigned char  base_mid;       // 8 bits: base address (middle byte)
    unsigned char  access;         // 8 bits: access byte flags
    unsigned char  limit_high_flag;// 4 bits: limit high + 4 bits: granularity flags
    unsigned char  base_high;      // 8 bits: base address (upper byte)
} __attribute__((packed)) gdt_entry_t;

// GDT pointer structure (6 bytes) - loaded by lgdt instruction
typedef struct {
    unsigned short limit;  // GDT size - 1
    unsigned int   base;   // GDT base address
} __attribute__((packed)) gdt_ptr_t;

// GDT table: 3 entries (null, code, data)
static gdt_entry_t gdt_entries[3];
static gdt_ptr_t   gdt_ptr;

// Access byte flags for GDT descriptors
#define GDT_PRESENT       0x80  // Segment is present in memory
#define GDT_RING0         0x00  // Privilege level 0 (kernel mode)
#define GDT_RING3         0x60  // Privilege level 3 (user mode)
#define GDT_CODE_DATA     0x10  // Descriptor type: code or data segment
#define GDT_EXECUTE       0x08  // Executable segment (code)
#define GDT_READABLE      0x02  // Readable/writable segment (data)

/**
 * gdt_set_gate() - Configure a single GDT descriptor entry
 * @num: Entry index in GDT table
 * @base: Base address of the memory segment
 * @limit: Size limit of the segment
 * @access: Access byte (privilege level, type, etc)
 * @gran: Granularity byte (4K granules, 32-bit mode, etc)
 */
static void gdt_set_gate(int num, unsigned int base, unsigned int limit,
                         unsigned char access, unsigned char gran) {
    // Split 32-bit base address into 3 parts (8, 8, 16 bits)
    gdt_entries[num].base_low       = (base & 0xFFFF);
    gdt_entries[num].base_mid       = (base >> 16) & 0xFF;
    gdt_entries[num].base_high      = (base >> 24) & 0xFF;
    
    // Split 20-bit limit into 2 parts (16, 4 bits)
    gdt_entries[num].limit_low      = (limit & 0xFFFF);
    gdt_entries[num].limit_high_flag = (limit >> 16) & 0x0F;
    
    // Add granularity flags to high limit bits
    gdt_entries[num].limit_high_flag |= (gran & 0xF0);
    
    // Set access byte
    gdt_entries[num].access         = access;
}

/**
 * gdt_install() - Initialize and load the Global Descriptor Table
 * 
 * This function sets up memory segmentation for x86 protected mode:
 * - Creates 3 descriptor entries (NULL, code, data)
 * - Loads the GDT pointer register with lgdt instruction
 * - Reloads segment registers to activate new segments
 * 
 * Called during kernel startup before enabling interrupts
 */
void gdt_install(void) {
    // Initialize GDT pointer (base address and limit)
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gdt_ptr.base  = (unsigned int)&gdt_entries;

    // Entry 0: NULL descriptor (required by CPU, unused)
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entry 1: Kernel code segment (selector 0x08)
    // - Base: 0x00000000 (flat address space)
    // - Limit: 0xFFFFFFFF (4 GB)
    // - Ring 0 (kernel), executable, readable
    // - 32-bit mode, 4KB granularity (0xC0)
    gdt_set_gate(1, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_RING0 | GDT_CODE_DATA | GDT_EXECUTE | GDT_READABLE,
                 0xC0);

    // Entry 2: Kernel data segment (selector 0x10)
    // - Base: 0x00000000 (flat address space)
    // - Limit: 0xFFFFFFFF (4 GB)
    // - Ring 0 (kernel), writable
    // - 32-bit mode, 4KB granularity (0xC0)
    gdt_set_gate(2, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_RING0 | GDT_CODE_DATA | GDT_READABLE,
                 0xC0);

    // Load the GDT pointer register using lgdt instruction
    __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));

    // Flush the instruction cache and reload segment registers
    // ljmp performs a far jump which reloads the code segment (CS)
    __asm__ __volatile__(
        "ljmp $0x08, $1f\n"      // Far jump to code segment (selector 0x08)
        "1:\n"
        "movw $0x10, %%ax\n"     // Load data segment selector (0x10) into AX
        "movw %%ax, %%ds\n"      // Set DS (data segment)
        "movw %%ax, %%es\n"      // Set ES (extra segment)
        "movw %%ax, %%fs\n"      // Set FS (F segment)
        "movw %%ax, %%gs\n"      // Set GS (G segment)
        "movw %%ax, %%ss\n"      // Set SS (stack segment)
        : : : "ax"               // Clobber AX register
    );
}

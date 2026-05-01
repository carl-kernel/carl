/* SPDX-License-Identifier: GPL-2.0-only 
* carl/mm/memory.c
* /* Copyright (C) MDFJ 2026
* MDFJ Project: Carl
* License: GNU General Public License v2.0. */

 
// Here we will control how much RAM the kernel will consume.

double total_ram = 0;

void init_memory() {
    // Get the total RAM from the multiboot information.
    total_ram = (double)multiboot_info->mem_lower + (double)multiboot_info->mem_upper;
}

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 // Enough to manage 128MB of RAM
#define BLOCK_ALIGN(sz) (((sz) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// The bitmap: each bit represents a 4KB page of RAM.
// 0 = Free, 1 = Used.
uint32_t memory_bitmap[BITMAP_SIZE];
uint32_t total_blocks = 0;
uint32_t used_blocks = 0;

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    // Align the size to the nearest page boundary.
    size = BLOCK_ALIGN(size);
    uint32_t blocks_needed = size / PAGE_SIZE;

    // Find a contiguous sequence of free blocks in the bitmap.
    uint32_t start_block = 0;
    uint32_t found_blocks = 0;

    for (uint32_t i = 0; i < total_blocks; i++) {
        if ((memory_bitmap[i / 32] & (1 << (i % 32))) == 0) {
            // This block is free.
            if (found_blocks == 0) {
                start_block = i; // Potential start of a free region.
            }
            found_blocks++;
            if (found_blocks == blocks_needed) {
                // We found enough contiguous blocks.
                for (uint32_t j = start_block; j < start_block + blocks_needed; j++) {
                    memory_bitmap[j / 32] |= (1 << (j % 32)); // Mark as used.
                }
                used_blocks += blocks_needed;
                return (void*)(start_block * PAGE_SIZE); // Return the address of the allocated memory.
            }
        } else {
            // This block is used, reset our search.
            found_blocks = 0;
        }
    }

    // If we reach here, we didn't find enough contiguous blocks.
    return NULL;
}

void kfree(void* ptr, size_t size) {
    if (ptr == NULL || size == 0) return;

    // Align the size to the nearest page boundary.
    size = BLOCK_ALIGN(size);
    uint32_t blocks_to_free = size / PAGE_SIZE;
    uint32_t start_block = (uint32_t)ptr / PAGE_SIZE;

    // Mark the blocks as free in the bitmap.
    for (uint32_t i = start_block; i < start_block + blocks_to_free; i++) {
        memory_bitmap[i / 32] &= ~(1 << (i % 32)); // Mark as free.
    }
    used_blocks -= blocks_to_free;
}

/* ============================================================================
 * MEMORY STATISTICS AND DEBUGGING
 * ============================================================================
 */
 
/**
 * memory_get_stats() - Get memory manager statistics
 *
 * @return: memory_stats_t structure with current statistics
 *
 * Provides information about heap usage and allocation history.
 */
memory_stats_t memory_get_stats(void) {
    memory_stats_t stats;
    
    stats.heap_start = (unsigned int)memory_manager.heap_start;
    stats.heap_end = (unsigned int)memory_manager.heap_end;
    stats.heap_used = (unsigned int)(memory_manager.heap_current - memory_manager.heap_start);
    stats.heap_total = KERNEL_HEAP_SIZE;
    stats.heap_free = stats.heap_total - stats.heap_used;
    stats.total_allocated = memory_manager.total_allocated;
    stats.total_freed = memory_manager.total_freed;
    stats.alloc_count = memory_manager.alloc_count;
    stats.free_count = memory_manager.free_count;
    
    return stats;
}
 
/**
 * memory_get_used() - Get total bytes currently allocated
 *
 * @return: Number of bytes allocated (not freed)
 */
unsigned int memory_get_used(void) {
    return memory_manager.total_allocated - memory_manager.total_freed;
}
 
/**
 * memory_get_free() - Get available heap space
 *
 * @return: Number of free bytes in heap
 */
unsigned int memory_get_free(void) {
    return (unsigned int)(memory_manager.heap_end - memory_manager.heap_current);
}
 
/**
 * memory_get_heap_size() - Get total heap size
 *
 * @return: Total heap size in bytes
 */
unsigned int memory_get_heap_size(void) {
    return KERNEL_HEAP_SIZE;
}
 
/**
 * memory_print_stats() - Print memory statistics to console
 *
 * Displays formatted memory usage information useful for debugging.
 * Called during kernel shutdown or on-demand.
 */
void memory_print_stats(void) {
    memory_stats_t stats = memory_get_stats();
    
    printf("\nMemory Statistics:\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("Heap Memory:\n");
    printf("  Start Address:  0x%08X\n", stats.heap_start);
    printf("  End Address:    0x%08X\n", stats.heap_end);
    printf("  Total Size:     %u bytes (%.2f MB)\n", 
           stats.heap_total, 
           (float)stats.heap_total / (1024 * 1024));
    printf("  Used:           %u bytes (%.2f MB)\n",
           stats.heap_used,
           (float)stats.heap_used / (1024 * 1024));
    printf("  Free:           %u bytes (%.2f MB)\n",
           stats.heap_free,
           (float)stats.heap_free / (1024 * 1024));
    printf("  Usage:          %.2f%%\n",
           (float)stats.heap_used / stats.heap_total * 100);
    
    printf("\nAllocation History:\n");
    printf("  Total Allocated: %u bytes\n", stats.total_allocated);
    printf("  Total Freed:     %u bytes\n", stats.total_freed);
    printf("  Allocations:     %u\n", stats.alloc_count);
    printf("  Deallocations:   %u\n", stats.free_count);
    printf("──────────────────────────────────────────────────────────────\n\n");
}
 
/* ============================================================================
 * MEMORY VALIDATION AND INTEGRITY CHECKING
 * ============================================================================
 */
 
/**
 * memory_validate_block() - Validate integrity of memory block
 *
 * @ptr: Pointer to validate
 * @return: 1 if valid, 0 if invalid
 *
 * Checks if memory block header is intact and magic number is valid.
 * Useful for detecting memory corruption.
 */
int memory_validate_block(void *ptr) {
    if (ptr == NULL) {
        return 0;
    }
    
    memory_block_t *block = (memory_block_t *)ptr - 1;
    
    // Check magic number
    if (block->magic != BLOCK_MAGIC) {
        return 0;
    }
    
    // Check if within allocated region
    if ((unsigned char *)ptr < memory_manager.heap_start ||
        (unsigned char *)ptr > memory_manager.heap_current) {
        return 0;
    }
    
    return 1;
}
 
/**
 * memory_dump() - Dump memory contents for debugging
 *
 * @ptr: Starting address to dump
 * @size: Number of bytes to dump
 *
 * Prints hex dump of memory region to console.
 * Useful for debugging memory corruption or examining data.
 */
void memory_dump(const void *ptr, unsigned int size) {
    unsigned char *data = (unsigned char *)ptr;
    unsigned int i;
    
    printf("Memory Dump at 0x%08X (%u bytes):\n", (unsigned int)ptr, size);
    printf("──────────────────────────────────────────────────────────────\n");
    
    for (i = 0; i < size; i += 16) {
        printf("%08X: ", (unsigned int)data + i);
        
        // Print hex values
        unsigned int j;
        for (j = 0; j < 16 && (i + j) < size; j++) {
            printf("%02X ", data[i + j]);
        }
        
        // Print ASCII representation
        printf(" | ");
        for (j = 0; j < 16 && (i + j) < size; j++) {
            unsigned char c = data[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("\n");
    }
    printf("──────────────────────────────────────────────────────────────\n\n");
}
 
/* ============================================================================
 * MEMORY INITIALIZATION AT BOOT
 * ============================================================================
 */
 
/**
 * FINAL NOTES:
 *
 * This memory manager provides:
 * 1. Dynamic memory allocation (malloc-like)
 * 2. Memory utilities (memset, memcpy, etc)
 * 3. Statistics and debugging support
 * 4. Simple heap management
 *
 * Limitations of this implementation:
 * - No memory coalescing (fragmentation possible)
 * - Simple linear allocation strategy
 * - No virtual memory or demand paging
 * - No multiprocessor synchronization
 *
 * Future improvements:
 * - Implement buddy system for efficient allocation
 * - Add memory fragmentation analysis
 * - Implement slab allocator for kernel objects
 * - Add per-process memory limits
 * - Implement demand paging and swap
 *
 * Integration with kernel:
 *   - Call memory_init() during kernel startup
 *   - Use memory_alloc/free for dynamic allocation
 *   - Monitor with memory_print_stats()
 */

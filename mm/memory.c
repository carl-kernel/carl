// © Gamer Mauri 2026
// [C] Bentosmau-gif 26

#include <stdio.h>
#include "memory.h"

static long total_swap = 0;
static long used_swap = 0;

void memory_init(long swap_kb) {
    total_swap = swap_kb;
    used_swap = 0;

    printf("SWAP ready: %ld KB\n", total_swap);
}

int memory_check(long requested_kb, long free_ram) {
    return requested_kb <= free_ram;
}

int memory_fallback(long kb_needed) {
    if (used_swap + kb_needed <= total_swap) {
        used_swap += kb_needed;

        printf("No available SWAP or RAM was found.\n");
        return -1;
    }

    void memory_free(long kb) {
        if (kb <= used_swap) {
            used_swap -= kb;
        } else {
            used_swap = 0;
        }
    }

    long get_used_swap() {
        return used_swap;
    }

    long get_free_swap() {
        return total_swap - used_swap;
        })

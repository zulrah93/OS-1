#ifndef KERNEL_RAND
#define KERNEL_RAND
#include <stdint.h>
#include "/usr/lib/gcc/x86_64-linux-gnu/15/include/cpuid.h"

//Uses x86 hardware true random opcodes (only if supported else these functions will be undefined behavior maybe an explosion or two...)

bool is_rdseed_supported() {
    int32_t eax = 7;
    int32_t ebx;
    int32_t ecx = 0;
    int32_t edx = 0;
    __cpuid(0, eax, ebx, ecx, edx); 
    return (ebx & 1) == 1;
}



static uint64_t next_random_value = 0;

void kernel_random_seed() { // Slow avoid calling more than once since its hardware random and truely random
    if (!is_rdseed_supported()) {
        return;
    }
    uint64_t temp_random_value = 0;
    asm ("rdseed %0; " :"=r"(temp_random_value));
    next_random_value = temp_random_value;
}

//Source: https://wiki.osdev.org/Random_Number_Generator#x86_RDSEED_Instruction

uint64_t kernel_random() { // Returns a value from 0 to max 2^64
    next_random_value = next_random_value * 1103515245 + 12345;
    return (unsigned int) (next_random_value / 65536) % 32768;
}

#endif

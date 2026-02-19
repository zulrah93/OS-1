#ifndef SYSTEM_H
#define SYSTEM_H
#include <memory.h>
#include "/usr/lib/gcc/x86_64-linux-gnu/15/include/cpuid.h"

#define CPU_MANU_STRING_LENGTH_PLUS_NULL 13

struct cpuid_t {
    char cpu_manufactuer_string[CPU_MANU_STRING_LENGTH_PLUS_NULL];
};
void get_cpu_information(struct cpuid_t* cpu_information) {
    
    if (!cpu_information) {
        return;
    }

    memset(cpu_information->cpu_manufactuer_string, '\0', sizeof(cpu_information->cpu_manufactuer_string));

    int32_t eax, ebx, ecx, edx;
    __cpuid(0, eax, ebx, ecx, edx); // level 0 returns a cpu manu string in the registers ebx, edx, and ecx respectively
    
    memcpy(cpu_information->cpu_manufactuer_string, &ebx, sizeof(ebx));
    memcpy(cpu_information->cpu_manufactuer_string + sizeof(ebx), &edx, sizeof(edx));
    memcpy(cpu_information->cpu_manufactuer_string + sizeof(ebx) + sizeof(edx), &ecx, sizeof(ecx));

    return;
}


#endif
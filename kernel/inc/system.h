#ifndef SYSTEM_H
#define SYSTEM_H
#include <memory.h>
#include <utility.h>

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

uint64_t get_cr0() {
    uint64_t register_cr0 = 0x0;
    asm ("movq %%cr0, %0; "
     :"=r"(register_cr0)    
     );
    return register_cr0;
}

char* get_cr0_as_heap_str(){
    uint64_t cr0 = get_cr0();
    char* cr0_str = (char*)k_malloc(12);
    if (NULL == cr0_str) {
        return NULL;
    }
    memset(cr0_str, 0, 12);
    integer_to_string(cr0_str, cr0);
    return cr0_str;
}

uint64_t get_cr3() {
    uint64_t register_cr3 = 0x0;
    asm ("movq %%cr3, %0; "
     :"=r"(register_cr3)    
     );
    return register_cr3;
}

char* get_cr3_as_heap_str(){
    uint64_t cr3 = get_cr3();
    char* cr3_str = (char*)k_malloc(12);
    if (NULL == cr3_str) {
        return NULL;
    }
    memset(cr3_str, 0, 12);
    integer_to_string(cr3_str, cr3);
    return cr3_str;
}



#endif
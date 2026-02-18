#ifndef SYSTEM_H
#define SYSTEM_H
#include <memory.h>
#include "/usr/lib/gcc/x86_64-linux-gnu/15/include/cpuid.h"

#define REGISTER_MAGIC 0xffff800000000200
#define CPU_MANU_STRING_LENGTH 12
#define CPU_MANU_STRING_LENGTH_PLUS_NULL 13

struct cpuid_t {
    char cpu_manufactuer_string[CPU_MANU_STRING_LENGTH_PLUS_NULL];
};
void get_cpu_information(struct cpuid_t* cpu_information) {
    
    if (!cpu_information) {
        return;
    }

    //TODO: Do something interesting rather than this
    memcpy(cpu_information->cpu_manufactuer_string, "PowaPC", string_length("PowaPC"));

    return;
}


#endif
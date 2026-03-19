#ifndef SYSTEM_H
#define SYSTEM_H
#include <memory.h>
#include <utility.h>
#include <io.h>

#include "/usr/lib/gcc/x86_64-linux-gnu/15/include/cpuid.h"

#define CPU_MANU_STRING_LENGTH_PLUS_NULL 13

#define CMOS_OUTPUT_PORT 0x70
#define CMOS_INPUT_PORT 0x71
#define SECONDS_REGISTER 0x0
#define MINUTES_REGISTER 0x2
#define HOURS_REGISTER 0x4
#define DAY_REGISTER 0x07
#define MONTH_REGISTER 0x08
#define YEAR_REGISTER 0x09

#define MAX_DATE_STRING_LENGTH 11 // Includes null char

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

static inline void set_cr0(uint64_t register_cr0) {
    asm ("movq %0, %%cr0; "
        :"=r"(register_cr0)    
    );
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
    char* cr0_str = (char*)k_malloc(14);
    if (NULL == cr0_str) {
        return NULL;
    }
    memset(cr0_str, 0, 14);
    cr0_str[0] = '0';
    cr0_str[1] = 'x';
    integer_to_hex(cr0_str + 2, cr0);
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
    char* cr3_str = (char*)k_malloc(14);
    if (NULL == cr3_str) {
        return NULL;
    }
    memset(cr3_str, 0, 14);
    cr3_str[0] = '0';
    cr3_str[1] = 'x';
    integer_to_hex(cr3_str + 2, cr3);
    return cr3_str;
}

typedef struct  {
    uint8_t month;
    uint8_t day;
    uint8_t year;
} rtc_date_t;

void get_real_time_date(rtc_date_t* date) {
    if (NULL == date) {
        return;
    }
    char month[3];
    memset(month, 0, sizeof(month));
    
    outb(MONTH_REGISTER, CMOS_OUTPUT_PORT);
    date->month = (uint8_t)inb(CMOS_INPUT_PORT);
    date->month = (date->month & 0x0F) + ((date->month / 16) * 10); // BCD to actual integer

    outb(DAY_REGISTER, CMOS_OUTPUT_PORT);
    date->day = (uint8_t)inb(CMOS_INPUT_PORT);
    date->day = (date->day & 0x0F) + ((date->day / 16) * 10); // BCD to actual integer

    outb(YEAR_REGISTER, CMOS_OUTPUT_PORT);
    date->year = (uint8_t)inb(CMOS_INPUT_PORT);
    date->year = (date->year & 0x0F) + ((date->year / 16) * 10); // BCD to actual integer
}


//Enters real mode to shutdown (experimental may blow the kernel call it if you are brave!)
void shutdown_pc() {
    uint64_t cr0_with_protected_mode_bit_disabled = get_cr0() & 0b11111111111111111111111111111110;
    set_cr0(cr0_with_protected_mode_bit_disabled);
    asm("mov $0x5301, %ax");
    asm("xor %bx, %bx");
    asm("int $0x15");
    asm("mov $0x530e, %ax");
    asm("xor %bx, %bx");
    asm("mov $0x0102, %cx");
    asm("int $0x15");
    asm("mov $0x5307, %ax");
    asm("mov $0x0001, %bx");
    asm("mov $0x0003, %cx");
    asm("int $0x15");
}



#endif
#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#pragma pack(push, 1)

typedef struct{
    uint16_t limit_address_low;       
    uint16_t base_address_low;        
    uint8_t  base_address_middle;     
    uint8_t  access_byte;     
    uint8_t  limit_high_flags;
    uint8_t  base_address_high;
} global_descriptor_entry_t ;

typedef struct{
    uint16_t limit;
    uint64_t base_address;

} global_descriptor_table_t ;

#pragma pack(pop)


#endif



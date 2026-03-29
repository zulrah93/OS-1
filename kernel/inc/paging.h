#ifndef PAGING_H
#define PAGING_H

#include <system.h>

#define PG_BIT 31
#define LA57_BIT 12
#define PCIDE_BIT 17

bool is_paging_enabled()
{
    return ((get_cr0() & (1 << PG_BIT)) >> PG_BIT) & 1;
}

bool is_5_level_page_table_supported()
{
    return ((get_cr4() & (1 << LA57_BIT)) >> LA57_BIT) & 1;
}

bool is_pcid_enabled() {
    return ((get_cr4() & (1 << PCIDE_BIT)) >> PCIDE_BIT) & 1;
}

uint64_t* get_root_page_directory_table() { 
    return (uint64_t*)(SLIDE_ADDRESS + ((get_cr3() & 0xfffffffffffff000) >> 0xfff));
}


#endif
#ifndef IDT_H
#define IDT_H

struct interrupt_descriptor_table_entry_t {
    uint16_t offset_low;    
    uint16_t selector;      
    uint8_t  interrupt_stack_table;           
    uint8_t  type_attr;     
    uint16_t offset_mid;    
    uint32_t offset_high;   
    uint32_t reserved;      
} __attribute__((packed));


struct interrupt_descriptor_table_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

#define IDT_ENTRIES 256

static struct interrupt_descriptor_table_entry_t entries[IDT_ENTRIES];
static struct interrupt_descriptor_table_t   interrupt_descriptor_table_register;


#define IDT_INTERRUPT_GATE 0x8E   
#define IDT_TRAP_GATE      0x8F   

#define KERNEL_CS 0x08            


static void idt_set_gate(uint8_t vector, void* handler,
                          uint16_t selector, uint8_t interrupt_stack_table, uint8_t type_attr)
{
    entries[vector].offset_low  = (uint16_t)((size_t)handler & 0xFFFF);
    entries[vector].selector    = selector;
    entries[vector].interrupt_stack_table         = interrupt_stack_table & 0x7;      
    entries[vector].type_attr   = type_attr;
    entries[vector].offset_mid  = (uint16_t)(((size_t)handler >> 16) & 0xFFFF);
    entries[vector].offset_high = (uint32_t)(((size_t)handler >> 32) & 0xFFFFFFFF);
    entries[vector].reserved    = 0;
}



struct interrupt_frame {
    uint64_t ip, cs, flags, sp, ss;
};

static struct limine_framebuffer * idt_framebuffer = (struct limine_framebuffer *)0x0;


__attribute__((interrupt))
static void divide_by_zero_handler(struct interrupt_frame *frame)
{
    asm("cli");
    asm("hlt");
}


__attribute__((interrupt))
static void page_fault_handler(struct interrupt_frame *frame, uint64_t error_code)
{
     asm("cli");
     asm("hlt");
}


void interrupt_descriptor_table_initialize(struct limine_framebuffer * framebuffer)
{

    idt_framebuffer = framebuffer;

    idt_set_gate(0,  divide_by_zero_handler,
                 KERNEL_CS, 0, IDT_INTERRUPT_GATE);
    idt_set_gate(14, page_fault_handler,
                 KERNEL_CS, 0, IDT_INTERRUPT_GATE);

    interrupt_descriptor_table_register.limit = sizeof(entries) - 1;
    interrupt_descriptor_table_register.base  = (uint64_t)&entries;
    asm("lidt %0" : : "m"(interrupt_descriptor_table_register));
    asm("sti");
}

#endif

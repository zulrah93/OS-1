#ifndef IDT_H
#define IDT_H

/*

Source: https://codeberg.org/Limine/limine-protocol/src/branch/trunk/PROTOCOL.md#x86-64-1

Machine State at Entry
x86-64

rip will be the entry point as defined as part of the executable file format, unless the Entry Point feature is requested, in which case, the value of rip is going to be taken from there.

At entry, CS is loaded with 0x28 and DS, ES, SS, FS, GS are loaded with 0x30, pointing to the 64-bit code and data descriptors respectively. In 64-bit mode, segment limits are not enforced and bases for CS, DS, ES, and SS are forced to zero. FS and GS bases are set to 0.

The GDT register is loaded to point to a GDT, in bootloader-reclaimable memory, with at least the following entries, starting at offset 0:

    Null descriptor
    16-bit code descriptor. Base = 0, limit = 0xffff. Readable.
    16-bit data descriptor. Base = 0, limit = 0xffff. Writable.
    32-bit code descriptor. Base = 0, limit = 0xffffffff. Readable.
    32-bit data descriptor. Base = 0, limit = 0xffffffff. Writable.
    64-bit code descriptor. Base = 0, limit irrelevant. Readable.
    64-bit data descriptor. Base = 0, limit irrelevant. Writable.

The IDT is in an undefined state. Executable must load its own.

*/

#pragma pack(push, 1)
typedef struct  {
    uint16_t offset_low;    
    uint16_t selector;      
    uint8_t  interrupt_stack_table;           
    uint8_t  type_attr;     
    uint16_t offset_mid;    
    uint32_t offset_high;   
    uint32_t reserved;      
} interrupt_descriptor_table_entry_t;


typedef struct {
    uint16_t limit;
    uint64_t base;
}interrupt_descriptor_table_t;

typedef struct {
    uint64_t ip, cs, flags, sp, ss;
}interrupt_frame_t;

#pragma pack(pop)

#define IDT_ENTRIES 256

static interrupt_descriptor_table_entry_t entries[IDT_ENTRIES];
static interrupt_descriptor_table_t   interrupt_descriptor_table_register;


#define IDT_INTERRUPT_GATE 0x8E      

#define KERNEL_CS 0x28            


static void idt_set_gate(uint8_t index, void* handler,
                          uint16_t selector)
{
    entries[index].offset_low  = (size_t)handler & 0xFFFF;
    entries[index].selector    = selector;
    entries[index].interrupt_stack_table         = 0;      
    entries[index].type_attr   = IDT_INTERRUPT_GATE;
    entries[index].offset_mid  = (((size_t)handler >> 16) & 0xFFFF);
    entries[index].offset_high = (((size_t)handler >> 32) & 0xFFFFFFFF);
    entries[index].reserved    = 0;
}

struct interrupt_frame_t
{
  uint64_t ip;
  uint64_t cs;
  uint64_t flags;
  uint64_t sp;
  uint64_t ss;
};

__attribute__((interrupt))
void general_interrupt_handler(struct interrupt_frame_t* frame)
{
    asm("cli");
    asm("hlt");
}

__attribute__((interrupt))
void general_fault_handler(struct interrupt_frame *frame, uint64_t error_code) {
    asm("cli");
    asm("hlt");
}


void interrupt_descriptor_table_initialize()
{
    idt_set_gate(1, general_fault_handler, KERNEL_CS);
    idt_set_gate(69, general_interrupt_handler, KERNEL_CS);

    interrupt_descriptor_table_register.limit = sizeof(entries) - 1;
    interrupt_descriptor_table_register.base  = (uint64_t)&entries[0];
    asm("lidt %0" : : "m"(interrupt_descriptor_table_register));
    asm("sti");
}

#endif

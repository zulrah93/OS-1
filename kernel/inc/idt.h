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
    uint8_t  type_attributes;     
    uint16_t offset_mid;    
    uint32_t offset_high;   
    uint32_t reserved;      
} interrupt_descriptor_table_entry_t;


typedef struct {
    uint16_t limit;
    uint64_t base;
}interrupt_descriptor_table_t;

typedef struct {
    uint64_t rax, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
}interrupt_stack_frame_t;

#pragma pack(pop)

#define IDT_ENTRIES 256

static interrupt_descriptor_table_entry_t entries[IDT_ENTRIES];
static interrupt_descriptor_table_t   interrupt_descriptor_table_register;


#define IDT_INTERRUPT_GATE_ATTRIBUTE 0x8E
#define IDT_FAULT_GATE_ATTRIBUTE 0x8F
#define PAGE_FAULT_INTERRUPT 0x0E         


static void idt_set_gate(uint8_t index, void* handler,
                          uint16_t selector, uint16_t attribute)
{
    entries[index].offset_low  = (uint32_t)handler & 0xFFFF;
    entries[index].selector    = selector;
    entries[index].interrupt_stack_table         = 0;      
    entries[index].type_attributes   = attribute;
    entries[index].offset_mid  = (((uint32_t)handler >> 16) & 0xFFFF);
    entries[index].offset_high = (((uint32_t)handler >> 32) & 0xFFFFFFFF);
    entries[index].reserved    = 0;
}

void general_interrupt_handler(interrupt_stack_frame_t* registers) {
    //asm("cli");
    //asm("hlt");
}

void general_fault_handler(interrupt_stack_frame_t* registers, uint64_t error_code) {
    asm("cli");
    asm("hlt");
}

void asm_interrupt_handler(void);
void asm_fault_handler(void);

void interrupt_descriptor_table_initialize()
{

    idt_set_gate(0, asm_fault_handler, get_code_segment_register(), IDT_FAULT_GATE_ATTRIBUTE);
    idt_set_gate(1, asm_interrupt_handler, get_code_segment_register(), IDT_FAULT_GATE_ATTRIBUTE);
    idt_set_gate(PAGE_FAULT_INTERRUPT, asm_fault_handler, get_code_segment_register(), IDT_FAULT_GATE_ATTRIBUTE);
    for(uint16_t interrupt_index =  0x2; interrupt_index <= 0xff; interrupt_index++) {
        switch(interrupt_index) {
            case 0x08: 
            case 0x11: 
            case 0x0A: 
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x15:
             {
                idt_set_gate(interrupt_index, asm_fault_handler, get_code_segment_register(), IDT_FAULT_GATE_ATTRIBUTE);
                break;
            }
            default: {
                idt_set_gate((uint8_t)interrupt_index, asm_interrupt_handler, get_code_segment_register(), IDT_INTERRUPT_GATE_ATTRIBUTE);
                break;
            }
        }
    }
    interrupt_descriptor_table_register.limit = sizeof(entries) - 1;
    interrupt_descriptor_table_register.base  = (uint64_t)entries;
    asm("lidt %0" : : "m"(interrupt_descriptor_table_register));
    asm("sti");
}

#endif

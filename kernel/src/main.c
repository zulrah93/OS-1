#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <memory.h>
#include <vga.h>
#include <fonts.h>
#include <system.h>
#include <keyboard.h>
#include <kernel_rand.h>
#include <kernel_string.h>

#define KERNEL_DEFAULT_FONT_COLOR from_rgb(0x82, 0x00,75)

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};



// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;


// Halt and catch fire function.
static void halt(const struct limine_framebuffer *framebuffer) {
    uint32_t color = BLUE;
    for (;;) {
        if (framebuffer) {
            fill_glyph(framebuffer, color);
        }
        else {
            asm("hlt");
        }
       
    }
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        halt(NULL);
    }

    uint64_t total_memory_size = 0;
    uint64_t total_usable_memory = 0;

    const size_t entry_count = memmap_request.response->entry_count; 
    struct limine_memmap_entry** entries = memmap_request.response->entries;

    for(size_t index = 0; index < entry_count; index++) {
        if (LIMINE_MEMMAP_USABLE == entries[index]->type) {
            total_usable_memory += entries[index]->length;
        }
        total_memory_size += entries[index]->length;
    }

    // Initialize memory pool
    init_memory_pool(entries, entry_count);

    kernel_random_seed();

   
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        halt(NULL);
    }

    //Enable keyboard for later use
    keyboard_encoder_enable();
    keyboard_controller_enable();
    if (!has_keyboard_controller_passed_self_test()) {
        halt(NULL);
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    bitmap_header_t* boot_logo_bmp_header = get_embedded_boot_logo();

    clear_screen(framebuffer, WHITE);
    if (NULL == boot_logo_bmp_header) {
        clear_screen(framebuffer, RED);
        // We're done, just hang...
        halt(framebuffer);
    }

    draw_bitmap(framebuffer, boot_logo_bmp_header, (framebuffer->width / 2) - 128, (framebuffer->height / 2) - 128);

    reset_cursor_position();

    struct cpuid_t cpuid;
    memset(&cpuid, 0, sizeof(cpuid));

    get_cpu_information(&cpuid);
    char* buffer = (char*)k_malloc(256); //[512];
    memset(buffer, '\0', 256);

    // kernel_string kernel_buffer;
    // create_empty_kernel_string(&kernel_buffer, 256);
    
    const char* welcome_text = "Welcome to OS/1!\nTotal System Memory (Bytes): ";
    uint32_t welcome_text_length = string_length(welcome_text);
    memcpy(buffer, welcome_text, welcome_text_length);
    integer_to_string(buffer + welcome_text_length, total_memory_size);
    uint32_t offset = string_length(buffer);
    
    const char* usable_memory_text = "\nUsable Memory (Bytes): ";
    memcpy(buffer + offset, usable_memory_text, string_length(usable_memory_text));
    offset = string_length(buffer);
    integer_to_string(buffer + offset, total_usable_memory);
    offset = string_length(buffer);
    
    const char* cpu_text = "\nCPU Detected: ";
    memcpy(buffer + offset, cpu_text, string_length(cpu_text));
    offset = string_length(buffer);
    memcpy(buffer + offset, cpuid.cpu_manufactuer_string, string_length(cpuid.cpu_manufactuer_string));
    
    printk(framebuffer, buffer, KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, (((get_cr0() & (1 << 31)) != 0) ? "\nPaging Enabled [Yes] and Kernel Heap Starts @ 0x" : "\nPaging Enabled [No] and Kernel Heap Starts @ 0x"), KERNEL_DEFAULT_FONT_COLOR);
    uint64_t start_of_heap_address = (uint64_t)start_of_heap;
    char heap_address_text[20];
    memset(heap_address_text, 0, sizeof(heap_address_text));
    integer_to_hex(heap_address_text, start_of_heap_address);
    printk(framebuffer, heap_address_text, KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, "\nCR0 has the value of ", KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, get_cr0_as_heap_str(), KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, " and CR3 has the value of ", KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, get_cr3_as_heap_str(), KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, " and the date is ", KERNEL_DEFAULT_FONT_COLOR);
    rtc_date_t date;
    memset(&date, 0, sizeof(date));
    get_real_time_date(&date);
    char month[3];
    memset(month, 0, sizeof(month));
    integer_to_string(month, date.month);
    printk(framebuffer, month, KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, "/", KERNEL_DEFAULT_FONT_COLOR);
    char day[3];
    memset(day, 0, sizeof(day));
    integer_to_string(day, date.day);
    printk(framebuffer, day, KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, "/", KERNEL_DEFAULT_FONT_COLOR);
    char year[3];
    memset(year, 0, sizeof(year));
    integer_to_string(year, date.year);
    printk(framebuffer, year, KERNEL_DEFAULT_FONT_COLOR);
    printk(framebuffer, " and is RDSEED instruction supported? ", KERNEL_DEFAULT_FONT_COLOR);
    bool rdseed_supported = is_rdseed_supported();
    printk(framebuffer, rdseed_supported ? " yes\n" : "no\n", KERNEL_DEFAULT_FONT_COLOR);
    if (rdseed_supported) {
        printk(framebuffer, "Lucky numbers for today's session are ", KERNEL_DEFAULT_FONT_COLOR);
        for (size_t _ = 0; _ < 10; _++) {
            char temp_lucky_number[5] = {0};
            integer_to_string(temp_lucky_number, kernel_random() % 1001);
            printk(framebuffer, temp_lucky_number, KERNEL_DEFAULT_FONT_COLOR);
            printk(framebuffer, " ", KERNEL_DEFAULT_FONT_COLOR);
        }
        printk(framebuffer, "\n", KERNEL_DEFAULT_FONT_COLOR);
        set_cursor_position(9, 0);
    }
    set_cursor_position(8, 0);
    kernel_string k_str;
    create_empty_kernel_string(&k_str, 10);
    append_c_str_to_kernel_string(&k_str, "$ ");
    print_kernel_string(framebuffer, k_str, from_rgb(0x82, 0x00, 0x4b));
    //printk(framebuffer, "$ ", from_rgb(0x82, 0x00, 0x4b));

    // uint8_t scan_code = poll_scan_code();
    // do {
    //     if (scan_code >= 1 && scan_code <= 58) {
    //         break;
    //     }
    //     if (scan_code >= 0x81 && scan_code <= 0xd8) {
    //         break;
    //     }
    // } while (scan_code)

    // We're done, just hang...
    halt(framebuffer);
}

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <memory.h>
#include <system.h>
#include <vga.h>
#include <idt.h>
#include <fonts.h>
#include <paging.h> // Includes system.h indirectly
#include <keyboard.h>
#include <kernel_rand.h>
#include <kernel_vector.h>
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

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        halt(NULL);
    }

    interrupt_descriptor_table_initialize();

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

    kernel_string kernel_buffer;
    create_empty_kernel_string(&kernel_buffer, 1024);

    append_c_str_to_kernel_string(&kernel_buffer, "Welcome to OS/1!\nTotal System Memory (Bytes): ");
    append_integer_to_kernel_string(&kernel_buffer, total_memory_size);
    
    append_c_str_to_kernel_string(&kernel_buffer, "\nUsable Memory (Bytes): ");
    append_integer_to_kernel_string(&kernel_buffer, total_usable_memory);
    
    append_c_str_to_kernel_string(&kernel_buffer, "\nCPU Detected: ");
    append_c_str_to_kernel_string(&kernel_buffer, cpuid.cpu_manufactuer_string);
    
    append_c_str_to_kernel_string(&kernel_buffer, (is_paging_enabled() ? "\nPaging Enabled [Yes] and Kernel Heap Starts @ 0x" 
                                                                                    : "\nPaging Enabled [No] and Kernel Heap Starts @ 0x"));
    append_hex_to_kernel_string(&kernel_buffer, (uint64_t)start_of_heap);
    
    if (is_5_level_page_table_supported()) {
        append_c_str_to_kernel_string(&kernel_buffer, " and using PML5T");
    }
    else {
        append_c_str_to_kernel_string(&kernel_buffer, " and using PML4T");
    }
    
    append_c_str_to_kernel_string(&kernel_buffer, "\n%cr0 has the value of ");
    append_c_str_to_kernel_string(&kernel_buffer,  get_cr0_as_heap_str());
    append_c_str_to_kernel_string(&kernel_buffer, " and %cr3 has the value of ");
    append_c_str_to_kernel_string(&kernel_buffer, get_cr3_as_heap_str());

    append_c_str_to_kernel_string(&kernel_buffer, " and qword [%cr3] == 0x");
    append_hex_to_kernel_string(&kernel_buffer, get_root_page_directory_table()[0]);
    
    append_c_str_to_kernel_string(&kernel_buffer, "\nThe date is ");
    
    rtc_date_t date;
    memset(&date, 0, sizeof(date));
    get_real_time_date(&date);
    append_integer_to_kernel_string(&kernel_buffer, date.month);
    append_c_str_to_kernel_string(&kernel_buffer, "/");
    append_integer_to_kernel_string(&kernel_buffer, date.day);
    append_c_str_to_kernel_string(&kernel_buffer, "/");
    append_integer_to_kernel_string(&kernel_buffer, date.year);
    append_c_str_to_kernel_string(&kernel_buffer, " and is RDSEED instruction supported? ");
    
    bool rdseed_supported = is_rdseed_supported();
    
    append_c_str_to_kernel_string(&kernel_buffer, rdseed_supported ? " yes\n" : "no\n");
    
    if (rdseed_supported) {
        append_c_str_to_kernel_string(&kernel_buffer, "Lucky numbers for today's session are ");
        for (size_t _ = 0; _ < 10; _++) {
            append_integer_to_kernel_string(&kernel_buffer, kernel_random() % 1001);
            append_c_str_to_kernel_string(&kernel_buffer, " ");
        }
    }

    append_c_str_to_kernel_string(&kernel_buffer, "\nSystem booted in ");

    uint64_t ticks = get_system_tick();
    append_integer_to_kernel_string(&kernel_buffer, ticks);
    append_c_str_to_kernel_string(&kernel_buffer, " tick(s) -- IA32_MPERF/IA32_APERF Present? ");
    append_c_str_to_kernel_string(&kernel_buffer, is_ia32_amperf_present() ? "yes and  running @ " : "no and running @ "); 
    append_integer_to_kernel_string(&kernel_buffer, get_cpu_frequency());
    append_c_str_to_kernel_string(&kernel_buffer, " MHz\n\n$ ");
    print_kernel_string(framebuffer, kernel_buffer, KERNEL_DEFAULT_FONT_COLOR);

    asm("int $1;");
    
    // We're done, just hang...
    halt(framebuffer);
}

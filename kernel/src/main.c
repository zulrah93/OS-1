#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <memory.h>
#include <limine.h>
#include <vga.h>
#include <fonts.h>
#include <system.h>

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
static volatile struct limine_efi_memmap_request efi_memmap_request = {
    .id = LIMINE_EFI_MEMMAP_REQUEST,
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
    uint32_t color = BLACK;
    for (;;) {
        if (framebuffer) {
            fill_glyph(framebuffer, color);
            // color ^= BLACK;
           // no_operation();
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

    if (efi_memmap_request.response) {
        total_memory_size = efi_memmap_request.response->memmap_size;
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        halt(NULL);
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    clear_screen(framebuffer, WHITE);
    reset_cursor_position();

    struct cpuid_t cpuid;
    memset(&cpuid, 0, sizeof(cpuid));

    get_cpu_information(&cpuid);
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));
    const char* welcome_text = "Welcome to OS/1!\nTotal System Memory (Bytes): ";
    uint32_t welcome_text_length = string_length(welcome_text);
    memcpy(buffer, welcome_text, welcome_text_length);
    integer_to_string(buffer + welcome_text_length, sizeof(buffer) - welcome_text_length, total_memory_size);
    uint32_t offset = string_length(buffer);
    const char* cpu_text = "\nCPU Detected: ";
    memcpy(buffer + offset, cpu_text, string_length(cpu_text));
    offset = string_length(buffer);
    memcpy(buffer + offset, cpuid.cpu_manufactuer_string, string_length(cpuid.cpu_manufactuer_string));
    printk(framebuffer, buffer, from_rgb(75, 0x00, 0x82));
    set_cursor_position(5, 0);
    printk(framebuffer, "$ ", from_rgb(0x4b, 0x00, 0x82));

    // We're done, just hang...
    halt(framebuffer);
}

#ifndef VGA_H
#define VGA_H

// Takes three bytes (red, green, blue) and returns a 32-bit integer representing the color
int32_t from_rgb(int8_t r, int8_t g, int8_t b) {
    return (r << 0) | (g << 8) | (b << 16);
}

#define RED 0xff0000
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define WHITE 0xffffff
#define BLACK 0x000000


void clear_screen(const struct limine_framebuffer * frame_buffer, const int32_t rgb) {
    if (!frame_buffer)
        return;

    uint32_t width = frame_buffer->width;
    uint32_t height = frame_buffer->height;

    int32_t* vga_buffer = (int32_t*)frame_buffer->address;

    if (!vga_buffer)
        return;
    
    for(uint32_t x = 0; x < (width*height); x++) {
        vga_buffer[x] = rgb;
    }
}


void plot_pixel(const struct limine_framebuffer * frame_buffer, const uint32_t x,  const uint32_t y, const int32_t rgb) {

    if (!frame_buffer)
        return;

    uint32_t width = frame_buffer->width;
    uint32_t height = frame_buffer->height;

    if (x > width) // Out of bounds
        return;
    if (y > height) // Out of bounds
        return;

    int32_t* vga_buffer = (int32_t*)frame_buffer->address;

    if (!vga_buffer)
        return;
    
    vga_buffer[(y * width) + x + 1] = rgb;
}


#endif
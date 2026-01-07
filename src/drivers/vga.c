/*
 * KontolOS VGA Text Mode Driver
 */

#include "vga.h"
#include "kernel.h"

/* VGA text mode buffer address */
#define VGA_BUFFER  0xB8000

/* Screen dimensions */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25

/* VGA I/O ports */
#define VGA_CTRL_PORT   0x3D4
#define VGA_DATA_PORT   0x3D5

/* Current cursor position */
static size_t vga_row = 0;
static size_t vga_col = 0;

/* Current color attribute */
static uint8_t vga_color = 0x0F;  /* White on black */

/* Pointer to VGA buffer */
static uint16_t *vga_buffer = (uint16_t *)VGA_BUFFER;

/*
 * Create a VGA entry (character + color)
 */
static inline uint16_t vga_entry(unsigned char c, uint8_t color)
{
    return (uint16_t)c | (uint16_t)color << 8;
}

/*
 * Initialize VGA driver
 */
void vga_init(void)
{
    vga_row = 0;
    vga_col = 0;
    vga_color = VGA_MAKE_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (uint16_t *)VGA_BUFFER;
}

/*
 * Clear the screen
 */
void vga_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_col = 0;
    vga_update_cursor();
}

/*
 * Set the current color
 */
void vga_set_color(enum vga_color fg, enum vga_color bg)
{
    vga_color = VGA_MAKE_COLOR(fg, bg);
}

/*
 * Scroll the screen up by one line
 */
static void vga_scroll(void)
{
    /* Move all lines up by one */
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t src = y * VGA_WIDTH + x;
            const size_t dst = (y - 1) * VGA_WIDTH + x;
            vga_buffer[dst] = vga_buffer[src];
        }
    }

    /* Clear the last line */
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = vga_entry(' ', vga_color);
    }

    vga_row = VGA_HEIGHT - 1;
}

/*
 * Print a single character
 */
void vga_putchar(char c)
{
    if (c == '\n') {
        vga_col = 0;
        vga_row++;
    } else if (c == '\r') {
        vga_col = 0;
    } else if (c == '\t') {
        /* Tab = 4 spaces */
        vga_col = (vga_col + 4) & ~3;
    } else if (c == '\b') {
        /* Backspace */
        if (vga_col > 0) {
            vga_col--;
            const size_t index = vga_row * VGA_WIDTH + vga_col;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    } else {
        const size_t index = vga_row * VGA_WIDTH + vga_col;
        vga_buffer[index] = vga_entry(c, vga_color);
        vga_col++;
    }

    /* Handle line wrap */
    if (vga_col >= VGA_WIDTH) {
        vga_col = 0;
        vga_row++;
    }

    /* Handle scrolling */
    if (vga_row >= VGA_HEIGHT) {
        vga_scroll();
    }

    vga_update_cursor();
}

/*
 * Print a string
 */
void vga_print(const char *str)
{
    while (*str) {
        vga_putchar(*str++);
    }
}

/*
 * Print a string with newline
 */
void vga_println(const char *str)
{
    vga_print(str);
    vga_putchar('\n');
}

/*
 * Print an integer in decimal
 */
void vga_print_dec(int32_t num)
{
    char buffer[12];
    int i = 0;
    bool negative = false;

    if (num < 0) {
        negative = true;
        num = -num;
    }

    if (num == 0) {
        vga_putchar('0');
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (negative) {
        vga_putchar('-');
    }

    while (--i >= 0) {
        vga_putchar(buffer[i]);
    }
}

/*
 * Print an integer in hexadecimal
 */
void vga_print_hex(uint32_t num)
{
    const char hex_chars[] = "0123456789ABCDEF";
    char buffer[9];
    int i;

    vga_print("0x");

    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    buffer[8] = '\0';

    /* Skip leading zeros */
    char *ptr = buffer;
    while (*ptr == '0' && *(ptr + 1) != '\0') {
        ptr++;
    }

    vga_print(ptr);
}

/*
 * Update the hardware cursor position
 */
void vga_update_cursor(void)
{
    uint16_t pos = vga_row * VGA_WIDTH + vga_col;

    outb(VGA_CTRL_PORT, 14);
    outb(VGA_DATA_PORT, (pos >> 8) & 0xFF);
    outb(VGA_CTRL_PORT, 15);
    outb(VGA_DATA_PORT, pos & 0xFF);
}

/*
 * Set cursor position
 */
void vga_set_cursor(size_t row, size_t col)
{
    if (row < VGA_HEIGHT && col < VGA_WIDTH) {
        vga_row = row;
        vga_col = col;
        vga_update_cursor();
    }
}

/*
 * Get current cursor row
 */
size_t vga_get_row(void)
{
    return vga_row;
}

/*
 * Get current cursor column
 */
size_t vga_get_col(void)
{
    return vga_col;
}

/*
 * Hide the hardware cursor
 */
void vga_hide_cursor(void)
{
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, 0x20);  /* Bit 5 set = cursor disabled */
}

/*
 * Show the hardware cursor
 */
void vga_show_cursor(void)
{
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, (inb(VGA_DATA_PORT) & 0xC0) | 14);  /* Cursor start scanline */
    outb(VGA_CTRL_PORT, 0x0B);
    outb(VGA_DATA_PORT, (inb(VGA_DATA_PORT) & 0xE0) | 15);  /* Cursor end scanline */
}

/*
 * Put a character at a specific position (without moving cursor)
 */
void vga_put_at(size_t row, size_t col, char c)
{
    if (row < VGA_HEIGHT && col < VGA_WIDTH) {
        const size_t index = row * VGA_WIDTH + col;
        vga_buffer[index] = vga_entry(c, vga_color);
    }
}

/*
 * Print a string at a specific position
 */
void vga_print_at(size_t row, size_t col, const char *str)
{
    size_t saved_row = vga_row;
    size_t saved_col = vga_col;
    
    vga_row = row;
    vga_col = col;
    
    while (*str && vga_col < VGA_WIDTH) {
        if (*str == '\n') {
            vga_row++;
            vga_col = col;  /* Reset to starting column */
        } else {
            vga_put_at(vga_row, vga_col, *str);
            vga_col++;
        }
        str++;
    }
    
    vga_row = saved_row;
    vga_col = saved_col;
}

/*
 * Print a string centered on a row
 */
void vga_print_centered(size_t row, const char *str)
{
    size_t len = 0;
    const char *p = str;
    while (*p) {
        len++;
        p++;
    }
    
    size_t col = (VGA_WIDTH > len) ? (VGA_WIDTH - len) / 2 : 0;
    vga_print_at(row, col, str);
}

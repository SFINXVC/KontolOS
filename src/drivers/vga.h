/*
 * KontolOS VGA Text Mode Driver Header
 */

#ifndef VGA_H
#define VGA_H

#include "../include/types.h"

/* VGA color codes */
enum vga_color {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN   = 14,  /* Yellow */
    VGA_COLOR_WHITE         = 15,
};

/* Create a color attribute byte */
#define VGA_MAKE_COLOR(fg, bg) ((bg) << 4 | (fg))

/* VGA driver functions */
void vga_init(void);
void vga_clear(void);
void vga_set_color(enum vga_color fg, enum vga_color bg);
void vga_putchar(char c);
void vga_print(const char *str);
void vga_println(const char *str);
void vga_print_dec(int32_t num);
void vga_print_hex(uint32_t num);
void vga_update_cursor(void);
void vga_set_cursor(size_t row, size_t col);
size_t vga_get_row(void);
size_t vga_get_col(void);

#endif /* VGA_H */

/*
 * KontolOS Kernel
 * Main kernel source file
 */

#include "kernel.h"
#include "vga.h"
#include "idt.h"
#include "keyboard.h"
#include "timer.h"
#include "shell.h"
#include "memory.h"
#include "../fs/ramfs.h"

/* Kernel version information */
#define KERNEL_VERSION "0.1.0"
#define KERNEL_NAME "KontolOS"

/* Forward declarations */
static void show_splash_screen(void);
static void init_system(void);

/*
 * Kernel main entry point
 * Called from kernel_entry.asm after protected mode setup
 */
void kernel_main(void)
{
    /* Initialize VGA text mode */
    vga_init();
    vga_clear();

    /* Early init: IDT and timer needed for splash screen animation */
    idt_init();
    timer_init(100);  /* 100 Hz timer for splash animation */

    /* Show splash screen with loading animation */
    show_splash_screen();

    /* Initialize remaining system components */
    init_system();

    /* Print ready message */
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("\n[+] System initialization complete!\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("[*] Starting shell...\n\n");

    /* Start the shell */
    shell_init();
    shell_run();

    /* Should never reach here */
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_print("\n[!] KERNEL PANIC: Shell exited unexpectedly!\n");

    /* Halt the system */
    for (;;) {
        __asm__ volatile("hlt");
    }
}

/*
 * Boot splash screen with loading animation
 */
static void show_splash_screen(void)
{
    /* Hide cursor during splash screen */
    vga_hide_cursor();
    
    /* Clear screen with black background */
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_clear();
    
    /* ASCII art logo - centered vertically (starting around row 6) */
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_centered(6,  "  _  __            _        _  ___  ____  ");
    vga_print_centered(7,  " | |/ /___  _ __ | |_ ___ | |/ _ \\/ ___| ");
    vga_print_centered(8,  " | ' // _ \\| '_ \\| __/ _ \\| | | | \\___ \\ ");
    vga_print_centered(9,  " | . \\ (_) | | | | || (_) | | |_| |___) |");
    vga_print_centered(10, " |_|\\_\\___/|_| |_|\\__\\___/|_|\\___/|____/ ");
    
    /* Version info */
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_print_centered(12, "Version 0.1.0");
    
    /* Tagline */
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_print_centered(14, "An operating system based on sistem reproduksi");
    
    /* Loading bar frame (row 20) */
    const size_t bar_row = 20;
    const size_t bar_width = 40;
    const size_t bar_start = (80 - bar_width - 2) / 2;  /* Center the bar */
    
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_put_at(bar_row, bar_start, '[');
    vga_put_at(bar_row, bar_start + bar_width + 1, ']');
    
    /* Loading text */
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_centered(22, "Loading...");
    
    /* Animate the loading bar over 3 seconds */
    /* Timer runs at 100Hz, so 300 ticks = 3 seconds */
    const uint32_t total_time_ms = 3000;
    const uint32_t step_time_ms = total_time_ms / bar_width;
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    for (size_t i = 0; i < bar_width; i++) {
        /* Fill in bar segment */
        vga_put_at(bar_row, bar_start + 1 + i, '#');
        
        /* Update percentage */
        int percent = ((i + 1) * 100) / bar_width;
        char percent_str[16];
        
        /* Simple int to string for percentage */
        if (percent == 100) {
            percent_str[0] = '1'; percent_str[1] = '0'; percent_str[2] = '0';
            percent_str[3] = '%'; percent_str[4] = '\0';
        } else if (percent >= 10) {
            percent_str[0] = '0' + (percent / 10);
            percent_str[1] = '0' + (percent % 10);
            percent_str[2] = '%'; percent_str[3] = '\0';
        } else {
            percent_str[0] = '0' + percent;
            percent_str[1] = '%'; percent_str[2] = '\0';
        }
        
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_print_centered(22, "            ");  /* Clear previous text */
        vga_print_centered(22, percent_str);
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        
        /* Wait for next step */
        timer_sleep_ms(step_time_ms);
    }
    
    /* Brief pause at 100% */
    timer_sleep_ms(200);
    
    /* Clear screen and show cursor */
    vga_clear();
    vga_show_cursor();
}

/*
 * Initialize all system components
 */
static void init_system(void)
{
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Initialize memory manager */
    vga_print("[*] Initializing memory manager... ");
    memory_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Initialize IDT (Interrupt Descriptor Table) */
    vga_print("[*] Setting up IDT... ");
    idt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Initialize PIT (Programmable Interval Timer) */
    vga_print("[*] Initializing timer... ");
    timer_init(100); /* 100 Hz */
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Initialize keyboard driver */
    vga_print("[*] Initializing keyboard... ");
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Initialize filesystem */
    vga_print("[*] Initializing filesystem... ");
    fs_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

/*
 * Kernel panic handler
 */
void kernel_panic(const char *message)
{
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_print("\n");
    vga_print("================================================================================");
    vga_print("                            KERNEL PANIC                                        ");
    vga_print("================================================================================");
    vga_print("\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("Error: ");
    vga_print(message);
    vga_print("\n\nSystem halted. Please restart your computer.\n");

    /* Disable interrupts and halt */
    __asm__ volatile("cli");
    for (;;) {
        __asm__ volatile("hlt");
    }
}

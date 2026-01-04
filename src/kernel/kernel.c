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

/* Kernel version information */
#define KERNEL_VERSION "0.1.0"
#define KERNEL_NAME "KontolOS"

/* Forward declarations */
static void print_boot_banner(void);
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

    /* Print boot banner */
    print_boot_banner();

    /* Initialize system components */
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
 * Print the boot banner
 */
static void print_boot_banner(void)
{
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("  _  __            _        _  ___  ____  \n");
    vga_print(" | |/ /___  _ __ | |_ ___ | |/ _ \\/ ___| \n");
    vga_print(" | ' // _ \\| '_ \\| __/ _ \\| | | | \\___ \\ \n");
    vga_print(" | . \\ (_) | | | | || (_) | | |_| |___) |\n");
    vga_print(" |_|\\_\\___/|_| |_|\\__\\___/|_|\\___/|____/ \n");
    vga_print("\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("  ");
    vga_print(KERNEL_NAME);
    vga_print(" Kernel v");
    vga_print(KERNEL_VERSION);
    vga_print("\n");
    vga_print("  An operating system based on sistem reproduksi\n");
    vga_print("\n");

    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_print("  ============================================\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
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

/*
 * KontolOS Shell
 */

#include "shell.h"
#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include "timer.h"
#include "memory.h"
#include "string.h"

/* Shell constants */
#define SHELL_BUFFER_SIZE   256
#define MAX_ARGS            16

/* Shell prompt */
static const char *shell_prompt = "kontol> ";

/* Command buffer */
static char command_buffer[SHELL_BUFFER_SIZE];

/* Command structure */
struct shell_command {
    const char *name;
    const char *description;
    void (*handler)(int argc, char *argv[]);
};

/* Forward declarations for command handlers */
static void cmd_help(int argc, char *argv[]);
static void cmd_clear(int argc, char *argv[]);
static void cmd_echo(int argc, char *argv[]);
static void cmd_info(int argc, char *argv[]);
static void cmd_uptime(int argc, char *argv[]);
static void cmd_memory(int argc, char *argv[]);
static void cmd_reboot(int argc, char *argv[]);
static void cmd_halt(int argc, char *argv[]);
static void cmd_version(int argc, char *argv[]);
static void cmd_color(int argc, char *argv[]);

/* Command table */
static struct shell_command commands[] = {
    { "help",    "Display available commands",       cmd_help },
    { "clear",   "Clear the screen",                 cmd_clear },
    { "echo",    "Echo text to the screen",          cmd_echo },
    { "info",    "Display system information",       cmd_info },
    { "uptime",  "Show system uptime",               cmd_uptime },
    { "memory",  "Display memory statistics",        cmd_memory },
    { "reboot",  "Reboot the system",                cmd_reboot },
    { "halt",    "Halt the system",                  cmd_halt },
    { "version", "Display OS version",               cmd_version },
    { "color",   "Change text color (0-15)",         cmd_color },
    { NULL, NULL, NULL }
};

/*
 * Initialize the shell
 */
void shell_init(void)
{
    /* Nothing to initialize for now */
}

/*
 * Parse command line into arguments
 */
static int parse_args(char *line, char *argv[], int max_args)
{
    int argc = 0;
    char *p = line;

    while (*p && argc < max_args) {
        /* Skip whitespace */
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        /* Store argument start */
        argv[argc++] = p;

        /* Find end of argument */
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }

        /* Null-terminate argument */
        if (*p) {
            *p++ = '\0';
        }
    }

    return argc;
}

/*
 * Execute a command
 */
static void execute_command(char *line)
{
    char *argv[MAX_ARGS];
    int argc = parse_args(line, argv, MAX_ARGS);

    if (argc == 0) {
        return;
    }

    /* Look up command */
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }

    /* Command not found */
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_print("Unknown command: ");
    vga_print(argv[0]);
    vga_print("\nType 'help' for available commands.\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

/*
 * Run the shell main loop
 */
void shell_run(void)
{
    while (1) {
        /* Print prompt */
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(shell_prompt);
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

        /* Read command */
        keyboard_readline(command_buffer, SHELL_BUFFER_SIZE);

        /* Execute command */
        execute_command(command_buffer);
    }
}

/*
 * Command: help
 */
static void cmd_help(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\nAvailable commands:\n");
    vga_print("-------------------\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    for (int i = 0; commands[i].name != NULL; i++) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("  ");
        vga_print(commands[i].name);
        vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        /* Pad to align descriptions */
        int len = strlen(commands[i].name);
        for (int j = len; j < 10; j++) {
            vga_putchar(' ');
        }
        vga_print(" - ");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_print(commands[i].description);
        vga_putchar('\n');
    }
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_putchar('\n');
}

/*
 * Command: clear
 */
static void cmd_clear(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    vga_clear();
}

/*
 * Command: echo
 */
static void cmd_echo(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (i > 1) vga_putchar(' ');
        vga_print(argv[i]);
    }
    vga_putchar('\n');
}

/*
 * Command: info
 */
static void cmd_info(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\n=== System Information ===\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    vga_print("  OS Name:        KontolOS\n");
    vga_print("  Version:        0.1.0\n");
    vga_print("  Architecture:   x86 (32-bit)\n");
    vga_print("  Author:         Built from scratch!\n");

    vga_print("\n  Memory Total:   ");
    vga_print_dec(memory_get_total() / 1024);
    vga_print(" KB\n");

    vga_print("  Memory Free:    ");
    vga_print_dec(memory_get_free() / 1024);
    vga_print(" KB\n");

    vga_print("  Uptime:         ");
    vga_print_dec(timer_get_uptime());
    vga_print(" seconds\n\n");
}

/*
 * Command: uptime
 */
static void cmd_uptime(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    uint32_t seconds = timer_get_uptime();
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;

    vga_print("Uptime: ");

    if (hours > 0) {
        vga_print_dec(hours);
        vga_print(" hours, ");
    }
    if (minutes > 0 || hours > 0) {
        vga_print_dec(minutes % 60);
        vga_print(" minutes, ");
    }
    vga_print_dec(seconds % 60);
    vga_print(" seconds\n");
}

/*
 * Command: memory
 */
static void cmd_memory(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    size_t total = memory_get_total();
    size_t used = memory_get_used();
    size_t free = memory_get_free();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\n=== Memory Statistics ===\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    vga_print("  Total:  ");
    vga_print_dec(total / 1024);
    vga_print(" KB (");
    vga_print_dec(total);
    vga_print(" bytes)\n");

    vga_print("  Used:   ");
    vga_print_dec(used / 1024);
    vga_print(" KB (");
    vga_print_dec(used);
    vga_print(" bytes)\n");

    vga_print("  Free:   ");
    vga_print_dec(free / 1024);
    vga_print(" KB (");
    vga_print_dec(free);
    vga_print(" bytes)\n\n");

    /* Show usage bar */
    int percent = (used * 100) / total;
    int bar_width = 40;
    int filled = (percent * bar_width) / 100;

    vga_print("  [");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    for (int i = 0; i < filled; i++) vga_putchar('#');
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    for (int i = filled; i < bar_width; i++) vga_putchar('-');
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("] ");
    vga_print_dec(percent);
    vga_print("%\n\n");
}

/*
 * Command: reboot
 */
static void cmd_reboot(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_print("Rebooting system...\n");

    /* Use keyboard controller to trigger a reset */
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);

    /* If that doesn't work, halt */
    halt();
}

/*
 * Command: halt
 */
static void cmd_halt(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_print("\nSystem halted. You may now turn off your computer.\n");

    disable_interrupts();
    for (;;) {
        halt();
    }
}

/*
 * Command: version
 */
static void cmd_version(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("KontolOS ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("v0.1.0\n");
    vga_print("Built from scratch with love!\n");
}

/*
 * Command: color
 */
static void cmd_color(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: color <0-15>\n");
        vga_print("Colors: 0=Black, 1=Blue, 2=Green, 3=Cyan, 4=Red, 5=Magenta\n");
        vga_print("        6=Brown, 7=LightGrey, 8=DarkGrey, 9=LightBlue\n");
        vga_print("        10=LightGreen, 11=LightCyan, 12=LightRed\n");
        vga_print("        13=LightMagenta, 14=Yellow, 15=White\n");
        return;
    }

    int color = 0;
    const char *p = argv[1];
    while (*p >= '0' && *p <= '9') {
        color = color * 10 + (*p - '0');
        p++;
    }

    if (color >= 0 && color <= 15) {
        vga_set_color((enum vga_color)color, VGA_COLOR_BLACK);
        vga_print("Text color changed!\n");
    } else {
        vga_print("Invalid color. Use 0-15.\n");
    }
}

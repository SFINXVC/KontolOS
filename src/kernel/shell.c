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
#include "../fs/ramfs.h"

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
static void cmd_shutdown(int argc, char *argv[]);
static void cmd_version(int argc, char *argv[]);
static void cmd_color(int argc, char *argv[]);
static void cmd_ls(int argc, char *argv[]);
static void cmd_cat(int argc, char *argv[]);
static void cmd_touch(int argc, char *argv[]);
static void cmd_rm(int argc, char *argv[]);
static void cmd_nano(int argc, char *argv[]);
static void cmd_mkdir(int argc, char *argv[]);
static void cmd_rmdir(int argc, char *argv[]);
static void cmd_cd(int argc, char *argv[]);
static void cmd_pwd(int argc, char *argv[]);

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
    { "shutdown","Power off the system",             cmd_shutdown },
    { "version", "Display OS version",               cmd_version },
    { "color",   "Change text color (0-15)",         cmd_color },
    { "ls",      "List files",                        cmd_ls },
    { "cat",     "Display file contents",             cmd_cat },
    { "touch",   "Create empty file",                 cmd_touch },
    { "rm",      "Remove file",                       cmd_rm },
    { "nano",    "Edit file",                         cmd_nano },
    { "mkdir",   "Create directory",                  cmd_mkdir },
    { "rmdir",   "Remove directory",                  cmd_rmdir },
    { "cd",      "Change directory",                  cmd_cd },
    { "pwd",     "Print working directory",           cmd_pwd },
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
 * Command: shutdown
 * Power off the system using ACPI
 */
static void cmd_shutdown(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\nShutting down KontolOS...\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_print("Goodbye!\n\n");

    /* Small delay for the message to be visible */
    for (volatile int i = 0; i < 10000000; i++);

    /* Disable interrupts */
    disable_interrupts();

    /* Try QEMU/Bochs ACPI power off (port 0x604, value 0x2000) */
    outw(0x604, 0x2000);

    /* Try older QEMU power off (port 0xB004, value 0x2000) */
    outw(0xB004, 0x2000);

    /* Try VirtualBox ACPI power off (port 0x4004, value 0x3400) */
    outw(0x4004, 0x3400);

    /* If ACPI power off didn't work, fall back to halt */
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_print("ACPI power off not supported. System halted.\n");
    vga_print("You may now turn off your computer manually.\n");

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

/*
 * Command: ls - List files
 */
static void cmd_ls(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int count = fs_count();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\nFiles in filesystem:\n");
    vga_print("--------------------\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    if (count == 0) {
        vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        vga_print("  (no files)\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        char buffer[1024];
        fs_list(buffer, sizeof(buffer));
        
        /* Print each file with size */
        char *line = buffer;
        while (*line) {
            char *end = line;
            while (*end && *end != '\n') end++;
            char saved = *end;
            *end = '\0';
            
            fs_file_t *f = fs_open(line);
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_print("  ");
            vga_print(line);
            
            if (f) {
                vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
                vga_print("  (");
                vga_print_dec((int)fs_get_size(f));
                vga_print(" bytes)");
            }
            vga_print("\n");
            
            *end = saved;
            if (*end) end++;
            line = end;
        }
    }
    
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_print("\nTotal: ");
    vga_print_dec(count);
    vga_print(" file(s)\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

/*
 * Command: cat - Display file contents
 */
static void cmd_cat(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: cat <filename>\n");
        return;
    }

    fs_file_t *file = fs_open(argv[1]);
    if (!file) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: File '");
        vga_print(argv[1]);
        vga_print("' not found\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }

    size_t size = fs_get_size(file);
    if (size == 0) {
        vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        vga_print("(empty file)\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }

    /* Read and display file contents */
    char *buffer = kmalloc(size + 1);
    if (buffer) {
        fs_read(file, buffer, size, 0);
        buffer[size] = '\0';
        vga_print(buffer);
        if (buffer[size - 1] != '\n') {
            vga_print("\n");
        }
        kfree(buffer);
    }
}

/*
 * Command: touch - Create empty file
 */
static void cmd_touch(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: touch <filename>\n");
        return;
    }

    int result = fs_create(argv[1]);
    if (result == 0) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("Created file '");
        vga_print(argv[1]);
        vga_print("'\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else if (result == -2) {
        vga_print("File '");
        vga_print(argv[1]);
        vga_print("' already exists\n");
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: Could not create file\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

/*
 * Command: rm - Remove file
 */
static void cmd_rm(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: rm <filename>\n");
        return;
    }

    int result = fs_delete(argv[1]);
    if (result == 0) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("Deleted file '");
        vga_print(argv[1]);
        vga_print("'\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: File '");
        vga_print(argv[1]);
        vga_print("' not found\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

/*
 * Command: nano - Simple text editor
 */
#define NANO_MAX_LINES  100
#define NANO_LINE_LEN   80

static void cmd_nano(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: nano <filename>\n");
        return;
    }

    const char *filename = argv[1];
    
    /* Create file if it doesn't exist */
    if (!fs_exists(filename)) {
        fs_create(filename);
    }

    /* Allocate text buffer */
    char **lines = kmalloc(NANO_MAX_LINES * sizeof(char *));
    if (!lines) {
        vga_print("Error: Out of memory\n");
        return;
    }
    
    for (int i = 0; i < NANO_MAX_LINES; i++) {
        lines[i] = kmalloc(NANO_LINE_LEN);
        if (lines[i]) {
            lines[i][0] = '\0';
        }
    }

    int num_lines = 1;
    int cur_line = 0;
    int cur_col = 0;
    int modified = 0;

    /* Load existing file content */
    fs_file_t *file = fs_open(filename);
    if (file && fs_get_size(file) > 0) {
        size_t size = fs_get_size(file);
        char *content = kmalloc(size + 1);
        if (content) {
            fs_read(file, content, size, 0);
            content[size] = '\0';
            
            /* Parse into lines */
            num_lines = 0;
            char *p = content;
            while (*p && num_lines < NANO_MAX_LINES) {
                char *line_start = p;
                while (*p && *p != '\n') p++;
                
                size_t len = p - line_start;
                if (len >= NANO_LINE_LEN) len = NANO_LINE_LEN - 1;
                
                strncpy(lines[num_lines], line_start, len);
                lines[num_lines][len] = '\0';
                num_lines++;
                
                if (*p == '\n') p++;
            }
            if (num_lines == 0) num_lines = 1;
            kfree(content);
        }
    }

    /* Editor main loop */
    int running = 1;
    while (running) {
        /* Draw screen */
        vga_clear();
        vga_hide_cursor();
        
        /* Title bar */
        vga_set_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
        vga_print("  KontolOS nano - ");
        vga_print(filename);
        if (modified) vga_print(" [modified]");
        for (int i = 40 + strlen(filename); i < 80; i++) vga_putchar(' ');
        
        /* Content area (lines 1-22) */
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        for (int i = 0; i < 22 && i < num_lines; i++) {
            vga_set_cursor(i + 1, 0);
            if (lines[i]) {
                vga_print(lines[i]);
            }
        }
        
        /* Status bar (line 23) */
        vga_set_cursor(23, 0);
        vga_set_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY);
        vga_print("  ^S Save  ^X Exit                                Line:");
        vga_print_dec(cur_line + 1);
        vga_print(" Col:");
        vga_print_dec(cur_col + 1);
        vga_print("          ");
        
        /* Position cursor */
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_set_cursor(cur_line + 1, cur_col);
        vga_show_cursor();
        
        /* Get input */
        char c = keyboard_getchar();
        
        /* Handle control keys */
        if (c == 19) {  /* Ctrl+S = Save */
            /* Build content string */
            size_t total_size = 0;
            for (int i = 0; i < num_lines; i++) {
                total_size += strlen(lines[i]) + 1;
            }
            
            char *content = kmalloc(total_size + 1);
            if (content) {
                content[0] = '\0';
                for (int i = 0; i < num_lines; i++) {
                    strcat(content, lines[i]);
                    strcat(content, "\n");
                }
                
                fs_file_t *f = fs_open(filename);
                if (f) {
                    fs_write(f, content, strlen(content));
                    modified = 0;
                }
                kfree(content);
            }
        } else if (c == 24) {  /* Ctrl+X = Exit */
            running = 0;
        } else if (c == '\b') {  /* Backspace */
            if (cur_col > 0) {
                cur_col--;
                size_t len = strlen(lines[cur_line]);
                for (size_t i = cur_col; i < len; i++) {
                    lines[cur_line][i] = lines[cur_line][i + 1];
                }
                modified = 1;
            } else if (cur_line > 0) {
                /* Merge with previous line */
                cur_line--;
                cur_col = strlen(lines[cur_line]);
                strcat(lines[cur_line], lines[cur_line + 1]);
                
                /* Shift lines up */
                for (int i = cur_line + 1; i < num_lines - 1; i++) {
                    strcpy(lines[i], lines[i + 1]);
                }
                lines[num_lines - 1][0] = '\0';
                num_lines--;
                modified = 1;
            }
        } else if (c == '\n' || c == '\r') {  /* Enter */
            if (num_lines < NANO_MAX_LINES) {
                /* Shift lines down */
                for (int i = num_lines; i > cur_line + 1; i--) {
                    strcpy(lines[i], lines[i - 1]);
                }
                num_lines++;
                
                /* Split current line */
                strcpy(lines[cur_line + 1], lines[cur_line] + cur_col);
                lines[cur_line][cur_col] = '\0';
                
                cur_line++;
                cur_col = 0;
                modified = 1;
            }
        } else if (c >= 32 && c < 127) {  /* Printable character */
            size_t len = strlen(lines[cur_line]);
            if (len < NANO_LINE_LEN - 1) {
                /* Insert character */
                for (size_t i = len + 1; i > (size_t)cur_col; i--) {
                    lines[cur_line][i] = lines[cur_line][i - 1];
                }
                lines[cur_line][cur_col] = c;
                cur_col++;
                modified = 1;
            }
        }
        
        /* Bounds checking */
        if (cur_line < 0) cur_line = 0;
        if (cur_line >= num_lines) cur_line = num_lines - 1;
        if (cur_col < 0) cur_col = 0;
        if (cur_col > (int)strlen(lines[cur_line])) {
            cur_col = strlen(lines[cur_line]);
        }
    }

    /* Cleanup */
    for (int i = 0; i < NANO_MAX_LINES; i++) {
        if (lines[i]) kfree(lines[i]);
    }
    kfree(lines);

    /* Restore screen */
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

/*
 * Command: mkdir - Create directory
 */
static void cmd_mkdir(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: mkdir <dirname>\n");
        return;
    }

    int result = fs_mkdir(argv[1]);
    if (result == 0) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("Created directory '");
        vga_print(argv[1]);
        vga_print("'\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else if (result == -2) {
        vga_print("Directory '");
        vga_print(argv[1]);
        vga_print("' already exists\n");
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: Could not create directory\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

/*
 * Command: rmdir - Remove directory
 */
static void cmd_rmdir(int argc, char *argv[])
{
    if (argc < 2) {
        vga_print("Usage: rmdir <dirname>\n");
        return;
    }

    int result = fs_rmdir(argv[1]);
    if (result == 0) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("Removed directory '");
        vga_print(argv[1]);
        vga_print("'\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else if (result == -2) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: '");
        vga_print(argv[1]);
        vga_print("' is not a directory\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: Directory '");
        vga_print(argv[1]);
        vga_print("' not found\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

/*
 * Command: cd - Change directory
 */
static void cmd_cd(int argc, char *argv[])
{
    const char *target = (argc < 2) ? "/" : argv[1];

    int result = fs_chdir(target);
    if (result == 0) {
        /* Success - silently change directory */
    } else if (result == -2) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: '");
        vga_print(target);
        vga_print("' is not a directory\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error: Directory '");
        vga_print(target);
        vga_print("' not found\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

/*
 * Command: pwd - Print working directory
 */
static void cmd_pwd(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vga_print(fs_getcwd());
    vga_print("\n");
}

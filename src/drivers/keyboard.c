/*
 * KontolOS Keyboard Driver
 */

#include "keyboard.h"
#include "idt.h"
#include "kernel.h"
#include "vga.h"

/* Keyboard I/O ports */
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64

/* Keyboard buffer */
#define KEYBOARD_BUFFER_SIZE    256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile size_t buffer_start = 0;
static volatile size_t buffer_end = 0;

/* Shift/Ctrl/Alt state */
static volatile bool shift_pressed = false;
static volatile bool ctrl_pressed = false;
static volatile bool alt_pressed = false;
static volatile bool capslock_on = false;

/* US QWERTY keyboard scancode to ASCII mapping (lowercase) */
static const char scancode_to_ascii[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' ', 0
};

/* US QWERTY keyboard scancode to ASCII mapping (uppercase/shifted) */
static const char scancode_to_ascii_shift[] = {
    0,    27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' ', 0
};

/* Key scancodes */
#define SCANCODE_ESCAPE         0x01
#define SCANCODE_BACKSPACE      0x0E
#define SCANCODE_TAB            0x0F
#define SCANCODE_ENTER          0x1C
#define SCANCODE_LCTRL          0x1D
#define SCANCODE_LSHIFT         0x2A
#define SCANCODE_RSHIFT         0x36
#define SCANCODE_LALT           0x38
#define SCANCODE_CAPSLOCK       0x3A
#define SCANCODE_F1             0x3B
#define SCANCODE_F2             0x3C
#define SCANCODE_F3             0x3D
#define SCANCODE_F4             0x3E
#define SCANCODE_F5             0x3F
#define SCANCODE_F6             0x40
#define SCANCODE_F7             0x41
#define SCANCODE_F8             0x42
#define SCANCODE_F9             0x43
#define SCANCODE_F10            0x44

/* Key release flag (bit 7 set) */
#define KEY_RELEASE_FLAG        0x80

/*
 * Add a character to the keyboard buffer
 */
static void buffer_put(char c)
{
    size_t next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

/*
 * Keyboard interrupt handler (IRQ1)
 */
static void keyboard_handler(struct interrupt_frame *frame)
{
    (void)frame;

    /* Read scancode from keyboard controller */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    /* Check if key release */
    bool released = (scancode & KEY_RELEASE_FLAG) != 0;
    scancode &= ~KEY_RELEASE_FLAG;

    /* Handle modifier keys */
    switch (scancode) {
        case SCANCODE_LSHIFT:
        case SCANCODE_RSHIFT:
            shift_pressed = !released;
            return;

        case SCANCODE_LCTRL:
            ctrl_pressed = !released;
            return;

        case SCANCODE_LALT:
            alt_pressed = !released;
            return;

        case SCANCODE_CAPSLOCK:
            if (!released) {
                capslock_on = !capslock_on;
            }
            return;
    }

    /* Only process key presses, not releases */
    if (released) {
        return;
    }

    /* Convert scancode to ASCII */
    if (scancode < sizeof(scancode_to_ascii)) {
        char c;

        /* Determine if we should use shifted characters */
        bool use_shift = shift_pressed;

        /* For letters, capslock toggles the shift state */
        if (scancode >= 0x10 && scancode <= 0x19) {  /* Q-P row */
            use_shift = shift_pressed ^ capslock_on;
        } else if (scancode >= 0x1E && scancode <= 0x26) {  /* A-L row */
            use_shift = shift_pressed ^ capslock_on;
        } else if (scancode >= 0x2C && scancode <= 0x32) {  /* Z-M row */
            use_shift = shift_pressed ^ capslock_on;
        }

        if (use_shift) {
            c = scancode_to_ascii_shift[scancode];
        } else {
            c = scancode_to_ascii[scancode];
        }

        if (c != 0) {
            buffer_put(c);
        }
    }
}

/*
 * Initialize keyboard driver
 */
void keyboard_init(void)
{
    /* Clear buffer */
    buffer_start = 0;
    buffer_end = 0;

    /* Register keyboard interrupt handler (IRQ1) */
    irq_register_handler(1, keyboard_handler);
}

/*
 * Check if a key is available in the buffer
 */
bool keyboard_has_key(void)
{
    return buffer_start != buffer_end;
}

/*
 * Get a key from the buffer (blocking)
 */
char keyboard_getchar(void)
{
    /* Wait for a key */
    while (!keyboard_has_key()) {
        halt();
    }

    /* Get character from buffer */
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;

    return c;
}

/*
 * Get a key from the buffer (non-blocking)
 */
char keyboard_getchar_nonblock(void)
{
    if (!keyboard_has_key()) {
        return 0;
    }

    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;

    return c;
}

/*
 * Read a line of input
 */
size_t keyboard_readline(char *buffer, size_t max_length)
{
    size_t i = 0;

    while (i < max_length - 1) {
        char c = keyboard_getchar();

        if (c == '\n') {
            vga_putchar('\n');
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                vga_putchar('\b');
            }
        } else {
            buffer[i++] = c;
            vga_putchar(c);
        }
    }

    buffer[i] = '\0';
    return i;
}

/*
 * Check if shift is pressed
 */
bool keyboard_shift_pressed(void)
{
    return shift_pressed;
}

/*
 * Check if ctrl is pressed
 */
bool keyboard_ctrl_pressed(void)
{
    return ctrl_pressed;
}

/*
 * Check if alt is pressed
 */
bool keyboard_alt_pressed(void)
{
    return alt_pressed;
}

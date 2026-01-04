/*
 * KontolOS Keyboard Driver Header
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../include/types.h"

/* Initialize keyboard driver */
void keyboard_init(void);

/* Check if a key is available */
bool keyboard_has_key(void);

/* Get a character (blocking) */
char keyboard_getchar(void);

/* Get a character (non-blocking, returns 0 if no key) */
char keyboard_getchar_nonblock(void);

/* Read a line of input */
size_t keyboard_readline(char *buffer, size_t max_length);

/* Check modifier key states */
bool keyboard_shift_pressed(void);
bool keyboard_ctrl_pressed(void);
bool keyboard_alt_pressed(void);

#endif /* KEYBOARD_H */

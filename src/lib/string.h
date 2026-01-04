/*
 * KontolOS String Functions Header
 */

#ifndef STRING_H
#define STRING_H

#include "../include/types.h"

/* String functions */
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);

/* Conversion functions */
char *itoa(int value, char *str, int base);
int atoi(const char *str);

/* Character classification */
int toupper(int c);
int tolower(int c);
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);
int isspace(int c);

#endif /* STRING_H */

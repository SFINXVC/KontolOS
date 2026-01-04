/*
 * KontolOS String Functions
 */

#include "string.h"

/*
 * Get string length
 */
size_t strlen(const char *s)
{
    size_t len = 0;
    while (s[len]) {
        len++;
    }
    return len;
}

/*
 * Compare two strings
 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/*
 * Compare n characters of two strings
 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/*
 * Copy string
 */
char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

/*
 * Copy n characters of string
 */
char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (n && (*d++ = *src++)) {
        n--;
    }
    while (n--) {
        *d++ = '\0';
    }
    return dest;
}

/*
 * Concatenate strings
 */
char *strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d) {
        d++;
    }
    while ((*d++ = *src++));
    return dest;
}

/*
 * Find character in string
 */
char *strchr(const char *s, int c)
{
    while (*s) {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }
    return (c == 0) ? (char *)s : NULL;
}

/*
 * Find last occurrence of character in string
 */
char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (c == 0) ? (char *)s : (char *)last;
}

/*
 * Find substring in string
 */
char *strstr(const char *haystack, const char *needle)
{
    if (!*needle) {
        return (char *)haystack;
    }

    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }

        if (!*n) {
            return (char *)haystack;
        }

        haystack++;
    }

    return NULL;
}

/*
 * Convert integer to string
 */
char *itoa(int value, char *str, int base)
{
    char *p = str;
    char *p1, *p2;
    int divisor;
    unsigned int uvalue;

    /* Handle negative numbers for base 10 */
    if (value < 0 && base == 10) {
        *p++ = '-';
        uvalue = (unsigned int)(-value);
    } else {
        uvalue = (unsigned int)value;
    }

    /* Calculate digits in reverse order */
    char *start = p;
    do {
        divisor = uvalue % base;
        *p++ = (divisor < 10) ? '0' + divisor : 'a' + divisor - 10;
        uvalue /= base;
    } while (uvalue);

    *p = '\0';

    /* Reverse the string */
    p1 = start;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return str;
}

/*
 * Convert string to integer
 */
int atoi(const char *str)
{
    int result = 0;
    int sign = 1;

    /* Skip whitespace */
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    /* Handle sign */
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    /* Convert digits */
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

/*
 * Convert character to uppercase
 */
int toupper(int c)
{
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}

/*
 * Convert character to lowercase
 */
int tolower(int c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

/*
 * Check if character is alphabetic
 */
int isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*
 * Check if character is digit
 */
int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

/*
 * Check if character is alphanumeric
 */
int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

/*
 * Check if character is whitespace
 */
int isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

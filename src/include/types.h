/*
 * KontolOS Type Definitions
 * ============================================================================
 * Our own type definitions - NO standard library dependencies at all!
 * This replaces stdint.h, stddef.h, stdbool.h
 * ============================================================================
 */

#ifndef TYPES_H
#define TYPES_H

/* ============================================================================
 * Boolean type
 * We define our own to avoid any standard library dependencies
 * ============================================================================ */
#ifndef __cplusplus
typedef unsigned char bool;
#define true  1
#define false 0
#endif

/* ============================================================================
 * Null pointer
 * ============================================================================ */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* ============================================================================
 * Size type (for memory operations)
 * On 32-bit x86, these are 32-bit values
 * ============================================================================ */
typedef unsigned int size_t;
typedef int          ssize_t;
typedef int          ptrdiff_t;

/* ============================================================================
 * Exact-width integer types
 * These are specific to x86-32 architecture
 * ============================================================================ */

/* Signed */
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

/* Unsigned */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

/* ============================================================================
 * Pointer-sized integer types
 * For 32-bit x86
 * ============================================================================ */
typedef int32_t  intptr_t;
typedef uint32_t uintptr_t;

/* ============================================================================
 * Limits
 * ============================================================================ */
#define INT8_MIN    (-128)
#define INT8_MAX    (127)
#define UINT8_MAX   (255U)

#define INT16_MIN   (-32768)
#define INT16_MAX   (32767)
#define UINT16_MAX  (65535U)

#define INT32_MIN   (-2147483647 - 1)
#define INT32_MAX   (2147483647)
#define UINT32_MAX  (4294967295U)

#define INT64_MIN   (-9223372036854775807LL - 1)
#define INT64_MAX   (9223372036854775807LL)
#define UINT64_MAX  (18446744073709551615ULL)

#define SIZE_MAX    UINT32_MAX

/* ============================================================================
 * Variadic arguments (compiler built-in, not library)
 * ============================================================================ */
typedef __builtin_va_list va_list;
#define va_start(v, l)  __builtin_va_start(v, l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v, l)    __builtin_va_arg(v, l)
#define va_copy(d, s)   __builtin_va_copy(d, s)

/* ============================================================================
 * Compiler attributes
 * ============================================================================ */
#define PACKED          __attribute__((packed))
#define ALIGNED(x)      __attribute__((aligned(x)))
#define NORETURN        __attribute__((noreturn))
#define UNUSED          __attribute__((unused))

#endif /* TYPES_H */

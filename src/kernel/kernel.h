/*
 * KontolOS Kernel Header
 */

#ifndef KERNEL_H
#define KERNEL_H

/* NO standard library - we use our own types! */
#include "../include/types.h"

/* Kernel version */
#define KONTOL_VERSION_MAJOR    0
#define KONTOL_VERSION_MINOR    1
#define KONTOL_VERSION_PATCH    0

/* Kernel panic function */
void kernel_panic(const char *message);

/* Port I/O functions */
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* I/O wait (slight delay) */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/* Enable/Disable interrupts */
static inline void enable_interrupts(void)
{
    __asm__ volatile("sti");
}

static inline void disable_interrupts(void)
{
    __asm__ volatile("cli");
}

/* Halt the CPU */
static inline void halt(void)
{
    __asm__ volatile("hlt");
}

#endif /* KERNEL_H */

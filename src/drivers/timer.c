/*
 * KontolOS Timer Driver (PIT - Programmable Interval Timer)
 */

#include "timer.h"
#include "idt.h"
#include "kernel.h"
#include "vga.h"

/* PIT ports */
#define PIT_CHANNEL0_DATA   0x40
#define PIT_CHANNEL1_DATA   0x41
#define PIT_CHANNEL2_DATA   0x42
#define PIT_COMMAND         0x43

/* PIT frequency */
#define PIT_BASE_FREQUENCY  1193182

/* Timer tick counter (using 32-bit to avoid 64-bit division which needs libgcc) */
static volatile uint32_t timer_ticks = 0;
static uint32_t timer_frequency = 0;

/*
 * Timer interrupt handler (IRQ0)
 */
static void timer_handler(struct interrupt_frame *frame)
{
    (void)frame;
    timer_ticks++;
}

/*
 * Initialize the timer
 */
void timer_init(uint32_t frequency)
{
    timer_frequency = frequency;
    timer_ticks = 0;

    /* Calculate divisor */
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    /* Send command byte */
    outb(PIT_COMMAND, 0x36);  /* Channel 0, lobyte/hibyte, rate generator */

    /* Send divisor */
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);         /* Low byte */
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);  /* High byte */

    /* Register timer interrupt handler (IRQ0) */
    irq_register_handler(0, timer_handler);
}

/*
 * Get the current tick count
 */
uint32_t timer_get_ticks(void)
{
    return timer_ticks;
}

/*
 * Get uptime in seconds
 */
uint32_t timer_get_uptime(void)
{
    return timer_ticks / timer_frequency;
}

/*
 * Sleep for a number of ticks
 */
void timer_sleep_ticks(uint32_t ticks)
{
    uint32_t end = timer_ticks + ticks;
    while (timer_ticks < end) {
        halt();
    }
}

/*
 * Sleep for a number of milliseconds
 */
void timer_sleep_ms(uint32_t ms)
{
    uint32_t ticks = (ms * timer_frequency) / 1000;
    if (ticks == 0) ticks = 1;
    timer_sleep_ticks(ticks);
}

/*
 * Sleep for a number of seconds
 */
void timer_sleep(uint32_t seconds)
{
    timer_sleep_ticks(seconds * timer_frequency);
}

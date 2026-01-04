/*
 * KontolOS Timer Driver Header
 */

#ifndef TIMER_H
#define TIMER_H

#include "../include/types.h"

/* Initialize timer with given frequency (Hz) */
void timer_init(uint32_t frequency);

/* Get current tick count */
uint32_t timer_get_ticks(void);

/* Get uptime in seconds */
uint32_t timer_get_uptime(void);

/* Sleep functions */
void timer_sleep_ticks(uint32_t ticks);
void timer_sleep_ms(uint32_t ms);
void timer_sleep(uint32_t seconds);

#endif /* TIMER_H */

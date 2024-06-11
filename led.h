#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include <stdint.h>

static const uint8_t kLedOneSecCnt = 120;

void led_init(void);

void led_timer_interrupt(void);

void led_main(void);

void led_is_dark_period(bool is_dark);

#endif // LED_H

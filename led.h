#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include <stdint.h>

static const uint8_t kLedOneSecCnt = 120;

unsigned char led_isr(void);

void led_init(void);

void led_timer_interrupt(void);

void led_main(void);

#endif // LED_H

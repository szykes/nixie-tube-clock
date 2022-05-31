#ifndef GPIO_H_
#define GPIO_H_

#include <avr/io.h>

static inline void gpio_init(void) {
  DDRB = (0x7 << 1);
}

static inline void gpio_set_led_red(void) {
  PORTB |= (1 << PORTB1);
}

static inline void gpio_reset_led_red(void) {
  PORTB &= ~(1 << PORTB1);
}


#endif // GPIO_H_

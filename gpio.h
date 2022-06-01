#ifndef GPIO_H_
#define GPIO_H_

#include <avr/io.h>

inline void gpio_init(void) {
  DDRB = (0x7 << 1);
  DDRB |= (1 << PB3) | (1 << PB5); // MOSI, SCK
}

inline void gpio_set_led_red(void) {
  PORTB |= (1 << PORTB1);
}

inline void gpio_reset_led_red(void) {
  PORTB &= ~(1 << PORTB1);
}

inline void gpio_set_led_green(void) {
  PORTB |= (1 << PORTB3);
}

inline void gpio_reset_led_green(void) {
  PORTB &= ~(1 << PORTB3);
}

inline void gpio_set_led_blue(void) {}

inline void gpio_reset_led_blue(void) {}

inline void gpio_latch_enable(void) {
// generate an impulse like this. Turn on the pin then turn it off
}

inline void gpio_set_blanking(void) {
// turn it on
}

inline void gpio_reset_blanking(void) {
// turn it off
}

inline void gpio_esp_reset(void) {
// turn it off then turn it on
}

#endif // GPIO_H_

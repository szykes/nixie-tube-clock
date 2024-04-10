#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "avr.h"

void mcu_sei(void) {
  sei();
}

void gpio_init(void) {
  DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB3) | (1 << DDB5); // MOSI, SCK
  DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2);
  DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD7);
}

void gpio_led_red_set(void) {
  PORTC |= (1 << PC0);
}

void gpio_led_red_reset(void) {
  PORTC &= ~(1 << PC0);
}

void gpio_led_green_set(void) {
  PORTC |= (1 << PC1);
}

void gpio_led_green_reset(void) {
  PORTC &= ~(1 << PC1);
}

void gpio_led_blue_set(void) {
  PORTC |= (1 << PC2);
}

void gpio_led_blue_reset(void) {
  PORTC &= ~(1 << PC2);
}

void gpio_data_set(void) {
  PORTB |= (1 << PB3);
}

void gpio_data_reset(void) {
  PORTB &= ~(1 << PB3);
}

void gpio_do_clk_cyc(void) {
  PORTB |= (1 << PB5);
  PORTB &= ~(1 << PB5);
}

void gpio_latch_enable_set(void) {
  PORTB |= (1 << PB0);
}

void gpio_latch_enable_reset(void) {
  PORTB &= ~(1 << PB0);
}

void gpio_blanking_set(void) {
  PORTB |= (1 << PB1);
}

void gpio_blanking_reset(void) {
  PORTB &= ~(1 << PB1);
}

void gpio_polarity_set(void) {
  PORTD |= (1 << PD7);
}

void gpio_polarity_reset(void) {
  PORTD &= ~(1 << PD7);
}

void gpio_esp_set(void) {
  PORTD |= (1 << PD2);
}

void gpio_esp_reset(void) {
  PORTD &= ~(1 << PD2);
}

void gpio_set_ch_pd(void) {
  PORTD |= (1 << PD3);
}

void gpio_reset_ch_pd(void) {
  PORTD &= ~(1 << PD3);
}

void wdt_init(void) {
  wdt_enable(WDTO_250MS);
}

void wdt_restart(void) {
  wdt_reset();
}

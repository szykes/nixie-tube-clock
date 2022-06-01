#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "gpio.h"
#include "wifi.h"
#include "clock.h"

#define MAX_CNT 250 // 31 250 Hz to 125 Hz

ISR(TIMER0_OVF_vect) {
  static char cnt = 0;

  if(cnt == MAX_CNT) {
    cnt = 0;
    //gpio_set_led_red();
    gpio_set_led_green();
    gpio_set_led_blue();
    clock_timer_interrupt();
  }

  if(cnt == wifi_get_led_red_ratio()) {
    //gpio_reset_led_red();
  }

  if(cnt == wifi_get_led_green_ratio()) {
    gpio_reset_led_green();
  }

  if(cnt == wifi_get_led_blue_ratio()) {
    gpio_reset_led_blue();
  }

  cnt++;

  TCNT0 = 0xFF;
}

void led_init(void) {
  // TCCR0A = 0x00;

  // 8 MHz internal RC osc. -> timer input: 31 250 Hz
  TCCR0B = (1 << CS02);

  TCNT0 = 0xFF;

  TIMSK0 = (1 << TOIE0);
}

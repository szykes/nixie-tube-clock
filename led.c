#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "gpio.h"
#include "wifi.h"
#include "clock.h"

#define MAX_CNT 240 // 28 800 Hz to 120 Hz

#define TIMER_INIT_CNT 0xFF

ISR(TIMER0_OVF_vect) {
  static char cnt = 0;

  if(cnt == MAX_CNT) {
    cnt = 0;
    gpio_set_led_red();
    gpio_set_led_green();
    gpio_set_led_blue();
    clock_timer_interrupt();
  }

  if(cnt == wifi_get_led_red_ratio()) {
    gpio_reset_led_red();
  }

  if(cnt == wifi_get_led_green_ratio()) {
    gpio_reset_led_green();
  }

  if(cnt == wifi_get_led_blue_ratio()) {
    gpio_reset_led_blue();
  }

  cnt++;

  TCNT0 = TIMER_INIT_CNT;
}

void led_init(void) {
  // TCCR0A = 0x00;

  // 7 372 800 Hz internal RC osc. / 256 -> timer input: 28 800 Hz
  TCCR0B = (1 << CS02);

  TCNT0 = TIMER_INIT_CNT;

  TIMSK0 = (1 << TOIE0);
}

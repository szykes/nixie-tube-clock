#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "gpio.h"
#include "wifi.h"
#include "clock.h"

#define MAX_CNT 240 // 28 800 Hz to 120 Hz

#define TIMER_INIT_CNT 0xFF

static volatile uint8_t red_ratio_wanted = 0;
static volatile uint8_t green_ratio_wanted = 0;
static volatile uint8_t blue_ratio_wanted = 0;

static volatile uint8_t red_ratio_current = 0;
static volatile uint8_t green_ratio_current = 0;
static volatile uint8_t blue_ratio_current = 20;

ISR(TIMER0_OVF_vect) {
  static uint8_t cnt = 0;

  if(cnt >= MAX_CNT) {
    cnt = 0;
    clock_timer_interrupt();

    if(red_ratio_current != 0) {
      gpio_led_red_set();
    }
    if(green_ratio_current != 0) {
      gpio_led_green_set();
    }
    if(blue_ratio_current != 0) {
      gpio_led_blue_set();
    }
  }

  if(cnt == red_ratio_current) {
    gpio_led_red_reset();
  }

  if(cnt == green_ratio_current) {
    gpio_led_green_reset();
  }

  if(cnt == blue_ratio_current) {
    gpio_led_blue_reset();
  }

  cnt++;

  TCNT0 = TIMER_INIT_CNT;
}

void led_init(void) {
  // TCCR0A = 0x00;

  // 7 372 800 Hz clk / 256 -> timer input: 28 800 Hz
  TCCR0 = (1 << CS02);

  TCNT0 = TIMER_INIT_CNT;

  TIMSK = (1 << TOIE0);
}

static void align_ratio_to_wanted(volatile uint8_t *wanted, volatile uint8_t *current) {
  if(*wanted > *current) {
    (*current)++;
  } else if(*wanted < *current) {
    (*current)--;
  }
}

void led_timer_interrupt(void) {
  align_ratio_to_wanted(&red_ratio_wanted, &red_ratio_current);
  align_ratio_to_wanted(&green_ratio_wanted, &green_ratio_current);
  align_ratio_to_wanted(&blue_ratio_wanted, &blue_ratio_current);
}

void led_set_red_ratio(uint8_t ratio) {
  red_ratio_wanted = ratio;
}

void led_set_green_ratio(uint8_t ratio) {
  green_ratio_wanted = ratio;
}

void led_set_blue_ratio(uint8_t ratio) {
  blue_ratio_wanted = ratio;
}

void led_is_dark_period(bool is_dark) {
  if(is_dark) {
    TCCR0 = 0x00;
    gpio_led_red_reset();
    gpio_led_green_reset();
    gpio_led_blue_reset();
  } else {
    TCCR0 = (1 << CS02);
  }
}

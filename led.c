#include "led.h"

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "avr.h"
#include "wifi.h"
#include "clock.h"

#define MAX_CNT 240 // 28 800 Hz to 120 Hz

#define TIMER_INIT_CNT 0xFF

#define HHM_TO_SECS(hour_10, hour_1, min_10) ( \
    (hour_10) * 10 * 3600 +				\
    (hour_1) * 3600 +					\
    (min_10) * 10 * 60)

#define BRIGHT_PERIOD_IN_SECS ( \
  HHM_TO_SECS(MAX_HOUR_10, MAX_HOUR_1, MAX_MIN_10) - \
  HHM_TO_SECS(MIN_HOUR_10, MIN_HOUR_1, MIN_MIN_10))

#define SEVENTH_OF_BRIGHT_PERIOD_IN_SECS (BRIGHT_PERIOD_IN_SECS / (float) 7)

#define RATIO_MODIFIER 40

static volatile bool is_calculate = false;

static volatile uint8_t red_ratio = 0;
static volatile uint8_t green_ratio = 0;
static volatile uint8_t blue_ratio = 0;

static uint32_t get_time_in_secs(void) {
  const time_st *time = clock_get_time();
  uint32_t secs = 0;
  secs += time->hour_10 * 10ul * 3600ul;
  secs += time->hour_1 * 3600ul;
  secs += time->min_10 * 10ul * 60ul;
  secs += time->min_1 * 60ul;
  secs += time->sec_10 * 10ul;
  secs += time->sec_1;
  return secs;
}

static uint8_t increasing_ratio(uint32_t base) {
  uint16_t ratio = roundf(base / (SEVENTH_OF_BRIGHT_PERIOD_IN_SECS / (float) (MAX_CNT + RATIO_MODIFIER)));
  return (ratio > MAX_CNT ? MAX_CNT : ratio);
}

static uint8_t decreasing_ratio(uint32_t base) {
  uint16_t ratio = roundf(base / (SEVENTH_OF_BRIGHT_PERIOD_IN_SECS / (float) (MAX_CNT + RATIO_MODIFIER)));
  return MAX_CNT - (ratio > MAX_CNT ? MAX_CNT : ratio);
}

unsigned char led_isr(void) {
  static uint8_t cnt = 0;

  if(cnt >= MAX_CNT) {
    cnt = 0;
    clock_timer_interrupt();

    if(red_ratio != 0) {
      gpio_led_red_set();
    }
    if(green_ratio != 0) {
      gpio_led_green_set();
    }
    if(blue_ratio != 0) {
      gpio_led_blue_set();
    }
  }

  if(cnt >= red_ratio) {
    gpio_led_red_reset();
  }

  if(cnt >= green_ratio) {
    gpio_led_green_reset();
  }

  if(cnt >= blue_ratio) {
    gpio_led_blue_reset();
  }

  cnt++;

  return TIMER_INIT_CNT;
}

void led_init(void) {
  timer0_init(TIMER_INIT_CNT);
}

void led_timer_interrupt(void) {
  is_calculate = true;
}

void led_main(void) {
  if(is_calculate) {
    is_calculate = false;

    int64_t secs = get_time_in_secs() - HHM_TO_SECS(MIN_HOUR_10, MIN_HOUR_1, MIN_MIN_10);
    if(secs < 0) {
      secs = 0;
    }
    uint32_t base = secs % ((uint32_t) SEVENTH_OF_BRIGHT_PERIOD_IN_SECS + 1);

    if(secs > (7 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = 0;
    } else if(secs > (6 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = decreasing_ratio(base);
    } else if(secs > (5 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = 0;
      green_ratio = decreasing_ratio(base);
      blue_ratio = MAX_CNT;
    } else if(secs > (4 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = 0;
      green_ratio = MAX_CNT;
      blue_ratio = increasing_ratio(base);
    } else if(secs > (3 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = decreasing_ratio(base);
      green_ratio = MAX_CNT;
      blue_ratio = 0;
    } else if(secs > (2 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      red_ratio = MAX_CNT;
      green_ratio = increasing_ratio(base);
      blue_ratio = 0;
    } else if(secs > SEVENTH_OF_BRIGHT_PERIOD_IN_SECS) {
      red_ratio = MAX_CNT;
      green_ratio = 0;
      blue_ratio = decreasing_ratio(base);
    } else if (secs > 0) {
      red_ratio = increasing_ratio(base);
      green_ratio = 0;
      blue_ratio = increasing_ratio(base);
    } else {
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = 0;
    }
  }
}

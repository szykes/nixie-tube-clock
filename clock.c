#include "clock.h"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "avr.h"
#include "led.h"
#include "wifi.h"

/*
time_data[0]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
M_1_2  | M_1_3  | M_1_4  | M_1_5  | M_1_6  | M_1_7  | M_1_8  | M_1_9

time_data[1]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
M_1_0  | M_1_1  | NC     | NC     | NC     | NC     | GLIM_3 | GLIM_2

time_data[2]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
S_10_5 | S_10_4 | S_10_3 | S_10_2 | S_10_0 | S_10_1 | S_1_2  | S_1_3

time_data[3]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
S_1_4  | S_1_5  | S_1_6  | S_1_7  | S_1_8  | S_1_9  | S_1_0  | S_1_1

time_data[4]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
NC     | NC     | NC     | NC     | NC     | H_10_2 | H_10_0 | H_10_1

time_data[5]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
NC     | NC     | NC     | NC     | NC     | NC     | H_1_2  | H_1_3

time_data[6]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
H_1_4  | H_1_5  | H_1_6  | H_1_7  | H_1_8  | H_1_9  | H_1_0  | H_1_1

time_data[7]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
GLIM_1 | GLIM_0 | M_10_5 | M_10_4 | M_10_3 | M_10_2 | M_10_0 | M_10_1
 */


static time_st time_data;
static uint8_t time_raw_data[8];

static volatile bool increment_time = false;
static volatile bool resetting_glimm = false;

static void clear_time_raw_data(void) {
  memset(&time_raw_data, 0x00, sizeof(time_raw_data));
}

static void latch_enable(void) {
  gpio_latch_enable_reset();
  gpio_latch_enable_set();
}

static void reset_glimm(void) {
  time_raw_data[1] &= (uint8_t)~(3 << 0);
  time_raw_data[7] &= (uint8_t)~(3 << 6);
}

static void set_glimm(void) {
  time_raw_data[1] |= (3 << 0);
  time_raw_data[7] |= (3 << 6);
}

static void set_hour_10(uint8_t hour) {
  switch(hour) {
  case 0:
    time_raw_data[4] |= (1 << 1);
    break;
  case 1:
    time_raw_data[4] |= (1 << 0);
    break;
  case 2:
    time_raw_data[4] |= (1 << 2);
    break;
  default:
    break;
  }
}

static void set_hour_1(uint8_t hour) {
  switch(hour) {
  case 0:
    time_raw_data[6] |= (1 << 1);
    break;
  case 1:
    time_raw_data[6] |= (1 << 0);
    break;
  case 2:
    time_raw_data[5] |= (1 << 1);
    break;
  case 3:
    time_raw_data[5] |= (1 << 0);
    break;
  case 4:
    time_raw_data[6] |= (1 << 7);
    break;
  case 5:
    time_raw_data[6] |= (1 << 6);
    break;
  case 6:
    time_raw_data[6] |= (1 << 5);
    break;
  case 7:
    time_raw_data[6] |= (1 << 4);
    break;
  case 8:
    time_raw_data[6] |= (1 << 3);
    break;
  case 9:
    time_raw_data[6] |= (1 << 2);
    break;
  default:
    break;
  }
}

static void set_min_10(uint8_t min) {
  switch(min) {
  case 0:
    time_raw_data[7] |= (1 << 1);
    break;
  case 1:
    time_raw_data[7] |= (1 << 0);
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    time_raw_data[7] |= (1 << min);
    break;
  default:
    break;
  }
}

static void set_min_1(uint8_t min) {
  switch(min) {
  case 0:
    time_raw_data[1] |= (1 << 7);
    break;
  case 1:
    time_raw_data[1] |= (1 << 6);
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    time_raw_data[0] |= (1 << (9 - min));
    break;
  default:
    break;
  }
}

static void set_sec_10(uint8_t sec) {
  switch(sec) {
  case 0:
    time_raw_data[2] |= (1 << 3);
    break;
  case 1:
    time_raw_data[2] |= (1 << 2);
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    time_raw_data[2] |= (1 << (sec + 2));
    break;
  default:
    break;
  }
}

static void set_sec_1(uint8_t sec) {
  switch(sec) {
  case 0:
    time_raw_data[3] |= (1 << 1);
    break;
  case 1:
    time_raw_data[3] |= (1 << 0);
    break;
  case 2:
    time_raw_data[2] |= (1 << 1);
    break;
  case 3:
    time_raw_data[2] |= (1 << 0);
    break;
  case 4:
    time_raw_data[3] |= (1 << 7);
    break;
  case 5:
    time_raw_data[3] |= (1 << 6);
    break;
  case 6:
    time_raw_data[3] |= (1 << 5);
    break;
  case 7:
    time_raw_data[3] |= (1 << 4);
    break;
  case 8:
    time_raw_data[3] |= (1 << 3);
    break;
  case 9:
    time_raw_data[3] |= (1 << 2);
    break;
  default:
    break;
  }
}

static void calculate_time(void) {
  clear_time_raw_data();

  time_data.sec_1++;

  if (time_data.sec_1 >= 10) {
    time_data.sec_1 = 0;
    time_data.sec_10++;
  }

  if (time_data.sec_10 >= 6) {
    time_data.sec_10 = 0;
    time_data.min_1++;
  }

  if (time_data.min_1 >= 10) {
    time_data.min_1 = 0;
    time_data.min_10++;
  }

  if (time_data.min_10 >= 6) {
    time_data.min_10 = 0;
    time_data.hour_1++;
  }

  if (time_data.hour_1 >= 10) {
    time_data.hour_1 = 0;
    time_data.hour_10++;
  }

  if ((time_data.hour_10 >= 2) && (time_data.hour_1 >= 4)) {
    time_data.hour_10 = 0;
    time_data.hour_1 = 0;
  }

  set_hour_10(time_data.hour_10);
  set_hour_1(time_data.hour_1);
  set_min_10(time_data.min_10);
  set_min_1(time_data.min_1);
  set_sec_10(time_data.sec_10);
  set_sec_1(time_data.sec_1);

  if (time_data.hour_10 == 1 &&
     time_data.hour_1 == 2 &&
     time_data.min_10 == 0 &&
     time_data.min_1 == 0 &&
     time_data.sec_10 == 0 &&
     time_data.sec_1 == 0) {
    wifi_query_timer();
  }
}

static void dark_period(void) {
  // turn off displaying between: 22:30 - 06:30
  if (((time_data.hour_10 == MAX_HOUR_10) && (time_data.hour_1 > MAX_HOUR_1)) ||
     ((time_data.hour_10 == MAX_HOUR_10) && (time_data.hour_1 == MAX_HOUR_1) && (time_data.min_10 >= MAX_MIN_10)) ||
     ((time_data.hour_10 == MIN_HOUR_10) && (time_data.hour_1 < MIN_HOUR_1)) ||
     ((time_data.hour_10 == MIN_HOUR_10) && (time_data.hour_1 == MIN_HOUR_1) && (time_data.min_10 < MIN_MIN_10))) {
    gpio_polarity_reset();
    gpio_blanking_set();
  } else {
    gpio_polarity_set();
    gpio_blanking_reset();
  }
}

static void transmit_bits(size_t idx) {
  for (int8_t i = CHAR_BIT - 1; i >= 0; i--) {
    if (time_raw_data[idx] & (1 << i)) {
      gpio_data_set();
    } else {
      gpio_data_reset();
    }
    gpio_do_clk_cyc();
  }
}

static void send_spi_time_data(void) {
  // SPI does not work well, the pins are floating.
  // I implemented myself.
  for (size_t i = 0; i < sizeof(time_raw_data); i++) {
    transmit_bits(i);
  }
  latch_enable();
}

void clock_init(void) {
  gpio_polarity_set();
  gpio_latch_enable_set();

  time_data.hour_10 = 2;
  time_data.hour_1 = 0;

  calculate_time();

  send_spi_time_data();
}

void clock_timer_interrupt(void) {
  static uint8_t cnt = 0;

  cnt++;

  if (cnt >= kLedOneSecCnt) {
    cnt = 0;
    increment_time = true;
    wifi_timer_interrupt();
    led_timer_interrupt();
  }

  if (cnt == kLedOneSecCnt / 2) {
    resetting_glimm = true;
  }
}

void clock_update_time(time_st accurate_time) {
  time_data = accurate_time;
}

const time_st *clock_get_time(void) {
  return &time_data;
}

void clock_main(void) {
  if (increment_time) {
    increment_time = false;

    calculate_time();

    dark_period();

    set_glimm();

    send_spi_time_data();
  }

  if (resetting_glimm) {
    resetting_glimm = false;

    reset_glimm();

    send_spi_time_data();
  }
}

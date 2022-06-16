#include "clock.h"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <avr/interrupt.h>

#include "gpio.h"
#include "led.h"

/*
time_data[0]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
M_1_9  | M_1_8  | M_1_7  | M_1_6  | M_1_5  | M_1_4  | M_1_3  | M_1_2

time_data[1]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
M_1_1  | M_1_0  | NC     | NC     | NC     | NC     | NC     | GLIM_1

time_data[2]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
S_10_5 | S_10_4 | S_10_3 | S_10_2 | S_10_1 | S_10_0 | S_1_9  | S_1_8

time_data[3]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
S_1_7  | S_1_6  | S_1_5  | S_1_4  | S_1_3  | S_1_2  | S_1_1  | S_1_0

time_data[4]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
NC     | NC     | NC     | NC     | NC     | H_10_2 | H_10_1 | H_10_0

time_data[5]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
NC     | NC     | NC     | NC     | NC     | NC     | H_1_9  | H_1_8

time_data[6]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
H_1_7  | H_1_6  | H_1_5  | H_1_4  | H_1_3  | H_1_2  | H_1_1  | H_1_0

time_data[7]
7      | 6      | 5      | 4      | 3      | 2      | 1      | 0
GLIM_0 | NC     | M_10_5 | M_10_4 | M_10_3 | M_10_2 | M_10_1 | M_10_0
 */

static time_st time_data;
static char time_raw_data[8];

static volatile bool increment_time = false;
static volatile bool resetting_glimm = false;

static void clear_time_data(void) {
  memset(&time_raw_data, 0, sizeof(time_raw_data));
}

static void set_glimm(void) {
  time_raw_data[1] |= (1 << 0);
  time_raw_data[7] |= (1 << 0);
}

static void reset_glimm(void) {
  time_raw_data[1] &= ~(1 << 0);
  time_raw_data[7] &= ~(1 << 0);
}

static void set_hour_10(char hour) {
  switch(hour) {
  case 0:
  case 1:
  case 2:
    time_raw_data[4] |= (1 << hour);
    break;
  default:
    break;
  }
}

static void set_hour_1(char hour) {
  switch(hour) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    time_raw_data[6] |= (1 << hour);
    break;
  case 8:
  case 9:
    time_raw_data[5] |= (1 << (hour - 8));
    break;
  default:
    break;
  }
}

static void set_min_10(char min) {
  switch(min) {
  case 0:
  case 1:
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

static void set_min_1(char min) {
  switch(min) {
  case 0:
  case 1:
    time_raw_data[1] |= (1 << (min + 6));
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    time_raw_data[0] |= (1 << (min - 2));
    break;
  default:
    break;
  }
}

static void set_sec_10(char sec) {
  switch(sec) {
  case 0:
  case 1:
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

static void set_sec_1(char sec) {
  switch(sec) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    time_raw_data[3] |= (1 << sec);
    break;
  case 8:
  case 9:
    time_raw_data[2] |= (1 << (sec - 8));
    break;
  default:
    break;
  }
}

static void calculate_time(void) {
  clear_time_data();

  time_data.sec_1++;

  if(time_data.sec_1 >= 10) {
    time_data.sec_1 = 0;
    time_data.sec_10++;
  }

  if(time_data.sec_10 >= 6) {
    time_data.sec_10 = 0;
    time_data.min_1++;
  }

  if(time_data.min_1 >= 10) {
    time_data.min_1 = 0;
    time_data.min_10++;
  }

  if(time_data.min_10 >= 6) {
    time_data.min_10 = 0;
    time_data.hour_1++;
  }

  if(time_data.hour_1 >= 10) {
    time_data.hour_1 = 0;
    time_data.hour_10++;
  }

  if(time_data.hour_10 >= 3) {
    time_data.hour_10 = 0;
  }

  set_hour_10(time_data.hour_10);
  set_hour_1(time_data.hour_1);
  set_min_10(time_data.min_10);
  set_min_1(time_data.min_1);
  set_sec_10(time_data.sec_10);
  set_sec_1(time_data.sec_1);
}

static void send_spi_time_data(size_t idx) {
  SPDR = time_raw_data[idx];
}

static void dark_period(void) {
  if((time_data.hour_10 <= 2) && (time_data.hour_1 <= 2) && (time_data.min_10 <= 3) &&
     (time_data.hour_10 == 0) && (time_data.hour_1 >= 6) && (time_data.min_10 >= 2)) {
    gpio_reset_blanking();
  } else {
    gpio_set_blanking();
  }
}

ISR(SPI_STC_vect) {
  static char time_data_idx = 0;

  time_data_idx++;

  if (time_data_idx >= sizeof(time_raw_data)) {
    time_data_idx = 0;
    gpio_latch_enable();
  } else {
    send_spi_time_data(time_data_idx);
  }
}

void clock_init(void) {
  SPCR = (1 << SPIE) | (1<<SPE) | (1<<MSTR) | (1 << CPHA) | (1<< SPR1) | (1<<SPR0);

  send_spi_time_data(0);
}

void clock_timer_interrupt(void) {
  static char cnt = 0;

  if(cnt >= ONE_SEC_CNT) {
    cnt = 0;
    increment_time = true;
  }

  if(cnt == ONE_SEC_CNT / 2) {
    resetting_glimm = true;
  }

  cnt++;
}

void clock_update_time(time_st accurate_time) {
  time_data = accurate_time;
}

void clock_main(void) {
  if(increment_time) {
    increment_time = false;

    calculate_time();

    set_glimm();

    send_spi_time_data(0);
  }

  if(resetting_glimm) {
    resetting_glimm = false;

    reset_glimm();

    send_spi_time_data(0);
  }

  dark_period();
}

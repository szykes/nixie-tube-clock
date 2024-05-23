#include "clock.h"

#include <limits.h>

#include "mock.h"
#include "framework.h"

#include "led.h"

// 20:12:55 ekkor nem láttam a LED-et

static uint8_t time_raw_data[8];

static void reset_glimm(void) {
  time_raw_data[1] &= ~(3 << 0);
  time_raw_data[7] &= ~(3 << 6);
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

static void set_bits(size_t idx) {
  for(int8_t i = CHAR_BIT - 1; i >= 0; i--) {
    if(time_raw_data[idx] & (1 << i)) {
      mock_initiate_expectation("gpio_data_set", NULL, 0, NULL);
    } else {
      mock_initiate_expectation("gpio_data_reset", NULL, 0, NULL);
    }
    mock_initiate_expectation("gpio_do_clk_cyc", NULL, 0, NULL);
  }
}

static void set_time(time_st time, bool is_glimm) {
  memset(&time_raw_data, 0x00, sizeof(time_raw_data));
  set_hour_10(time.hour_10);
  set_hour_1(time.hour_1);
  set_min_10(time.min_10);
  set_min_1(time.min_1);
  set_sec_10(time.sec_10);
  set_sec_1(time.sec_1);

  if (is_glimm) {
    set_glimm();
  } else {
    reset_glimm();
  }

  for(size_t i = 0; i < sizeof(time_raw_data); i++) {
    set_bits(i);
  }

  mock_initiate_expectation("gpio_latch_enable_reset", NULL, 0, NULL);
  mock_initiate_expectation("gpio_latch_enable_set", NULL, 0, NULL);
}

static void set_time_sequence(time_st expected_time, time_st *current_time, bool is_increment_time, bool is_dark_period, int modifier) {
  if (current_time != NULL) {
    clock_update_time(*current_time);
  }

  for (int cnt = 0; cnt < ((kLedOneSecCnt / 2) - modifier); cnt++) {
    clock_timer_interrupt();
    clock_main();
  }

  if (is_increment_time) {
    mock_initiate_expectation("wifi_timer_interrupt", NULL, 0, NULL);
    mock_initiate_expectation("led_timer_interrupt", NULL, 0, NULL);
  }

  clock_timer_interrupt();

  if (is_increment_time) {
    if (is_dark_period) {
      mock_initiate_expectation("gpio_polarity_reset", NULL, 0, NULL);
      mock_initiate_expectation("gpio_blanking_set", NULL, 0, NULL);

      bool is_dark_period = true;
      void *param_ptr;
      mock_prepare_param(param_ptr, is_dark_period);

      type_st params[] = {
	{
	  .type = TYPE_BOOL,
	  .value = param_ptr,
	  .size = sizeof(bool),
	},
      };
      mock_initiate_expectation("led_is_dark_period", params, sizeof(params)/sizeof(type_st), NULL);
    } else {
      mock_initiate_expectation("gpio_polarity_set", NULL, 0, NULL);
      mock_initiate_expectation("gpio_blanking_reset", NULL, 0, NULL);

      bool is_dark_period = false;
      void *param_ptr;
      mock_prepare_param(param_ptr, is_dark_period);

      type_st params[] = {
	{
	  .type = TYPE_BOOL,
	  .value = param_ptr,
	  .size = sizeof(bool),
	},
      };
      mock_initiate_expectation("led_is_dark_period", params, sizeof(params)/sizeof(type_st), NULL);
    }
  }

  set_time(expected_time, is_increment_time);

  clock_main();
}

static void tc_clock_init(void) {
  TEST_BEGIN();

  mock_initiate_expectation("gpio_polarity_set", NULL, 0, NULL);
  mock_initiate_expectation("gpio_latch_enable_set", NULL, 0, NULL);

  set_time((time_st) {
      .hour_10 = 2,
      .hour_1 = 0,
      .min_10 = 0,
      .min_1 = 0,
      .sec_10 = 0,
      .sec_1 = 1,
    }, false);

  clock_init();

  const time_st *actual_time = clock_get_time();
  TEST_ASSERT(actual_time->hour_10 == 2, "tc_clock_init - hour_10");
  TEST_ASSERT(actual_time->hour_1 == 0, "tc_clock_init - hour_1");
  TEST_ASSERT(actual_time->min_10 == 0, "tc_clock_init - min_10");
  TEST_ASSERT(actual_time->min_1 == 0, "tc_clock_init - min_1");
  TEST_ASSERT(actual_time->sec_10 == 0, "tc_clock_init - sec_10");
  TEST_ASSERT(actual_time->sec_1 == 1, "tc_clock_init - sec_1");

  TEST_END();
}

static void tc_set_get_time(void) {
  TEST_BEGIN();

  time_st time = {
    .hour_10 = 2,
    .hour_1 = 0,
    .min_10 = 5,
    .min_1 = 9,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  clock_update_time(time);
  const time_st *actual_time = clock_get_time();
  TEST_ASSERT(actual_time->hour_10 == 2, "tc_change_time - hour_10");
  TEST_ASSERT(actual_time->hour_1 == 0, "tc_change_time - hour_1");
  TEST_ASSERT(actual_time->min_10 == 5, "tc_change_time - min_10");
  TEST_ASSERT(actual_time->min_1 == 9, "tc_change_time - min_1");
  TEST_ASSERT(actual_time->sec_10 == 0, "tc_change_time - sec_10");
  TEST_ASSERT(actual_time->sec_1 == 0, "tc_change_time - sec_1");

  TEST_END();
}

static void tc_counting_clock_in_dark_period(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = 2,
    .hour_1 = 0,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = 2,
    .hour_1 = 3,
    .min_10 = 5,
    .min_1 = 1,
    .sec_10 = 4,
    .sec_1 = 5,
  };

  set_time_sequence(expected_time, &current_time, false, true, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, true, 0);

  TEST_END();
}

static void tc_counting_clock_in_bright_period(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = 2,
    .hour_1 = 3,
    .min_10 = 5,
    .min_1 = 1,
    .sec_10 = 4,
    .sec_1 = 6,
  };

  time_st current_time = {
    .hour_10 = 1,
    .hour_1 = 2,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  set_time_sequence(expected_time, &current_time, false, false, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, false, 1);

  TEST_END();
}

static void tc_dark_period_after_midnight(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = 1,
    .hour_1 = 2,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = 0,
    .hour_1 = 0,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  set_time_sequence(expected_time, &current_time, false, true, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, true, 1);

  TEST_END();
}

static void tc_dark_period_before_min_time(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = 0,
    .hour_1 = 0,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = MIN_HOUR_10,
    .hour_1 = MIN_HOUR_1,
    .min_10 = MIN_MIN_10 - 1,
    .min_1 = 9,
    .sec_10 = 5,
    .sec_1 = 8,
  };

  set_time_sequence(expected_time, &current_time, false, true, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, true, 1);

  TEST_END();
}

static void tc_bright_period_at_min_time(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = MIN_HOUR_10,
    .hour_1 = MIN_HOUR_1,
    .min_10 = MIN_MIN_10 - 1,
    .min_1 = 9,
    .sec_10 = 5,
    .sec_1 = 9,
  };

  time_st current_time = {
    .hour_10 = MIN_HOUR_10,
    .hour_1 = MIN_HOUR_1,
    .min_10 = MIN_MIN_10,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  set_time_sequence(expected_time, &current_time, false, false, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, false, 1);

  TEST_END();
}

static void tc_bright_period_at_noon(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = MIN_HOUR_10,
    .hour_1 = MIN_HOUR_1,
    .min_10 = MIN_MIN_10,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = 1,
    .hour_1 = 2,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  set_time_sequence(expected_time, &current_time, false, false, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, false, 1);

  TEST_END();
}

static void tc_bright_period_at_max_time(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = 1,
    .hour_1 = 2,
    .min_10 = 0,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = MAX_HOUR_10,
    .hour_1 = MAX_HOUR_1,
    .min_10 = MAX_MIN_10 - 1,
    .min_1 = 9,
    .sec_10 = 5,
    .sec_1 = 8,
  };

  set_time_sequence(expected_time, &current_time, false, false, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, false, 1);

  TEST_END();
}

static void tc_dark_period_after_max_time(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = MAX_HOUR_10,
    .hour_1 = MAX_HOUR_1,
    .min_10 = MAX_MIN_10 - 1,
    .min_1 = 9,
    .sec_10 = 5,
    .sec_1 = 9,
  };

  time_st current_time = {
    .hour_10 = MAX_HOUR_10,
    .hour_1 = MAX_HOUR_1,
    .min_10 = MAX_MIN_10,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 0,
  };

  set_time_sequence(expected_time, &current_time, false, true, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, true, 1);

  TEST_END();
}

static void tc_dark_period_before_midnight(void) {
  TEST_BEGIN();

  time_st expected_time = {
    .hour_10 = MAX_HOUR_10,
    .hour_1 = MAX_HOUR_1,
    .min_10 = MAX_MIN_10,
    .min_1 = 0,
    .sec_10 = 0,
    .sec_1 = 1,
  };

  time_st current_time = {
    .hour_10 = 2,
    .hour_1 = 3,
    .min_10 = 5,
    .min_1 = 9,
    .sec_10 = 5,
    .sec_1 = 8,
  };

  set_time_sequence(expected_time, &current_time, false, true, 1);
  current_time.sec_1++;
  set_time_sequence(current_time, NULL, true, true, 1);

  TEST_END();
}

int main(void) {
  tc_clock_init();
  tc_set_get_time();
  tc_counting_clock_in_dark_period();
  tc_counting_clock_in_bright_period();
  tc_dark_period_after_midnight();
  tc_dark_period_before_min_time();
  tc_bright_period_at_min_time();
  tc_bright_period_at_noon();
  tc_bright_period_at_max_time();
  tc_dark_period_after_max_time();
  tc_dark_period_before_midnight();
  return 0;
}

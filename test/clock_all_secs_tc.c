#include "clock.h"

#include <limits.h>

#include "mock.h"
#include "framework.h"
#include "clock_gen.h"

#include "led.h"

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

  static bool is_first = true;
  if (is_first) {
    memset(&time_raw_data, 0x00, sizeof(time_raw_data));
  }
  is_first = false;

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

static bool set_half_second(time_st time, bool is_increment_time, bool is_dark_period) {
  static int modifier = 0;

  for (int cnt = 0; cnt < ((kLedOneSecCnt / 2) - modifier); cnt++) {
    clock_timer_interrupt();
    clock_main();
  }

  static bool is_first = true;
  if (is_first) {
    modifier = 1;
  }
  is_first = false;

  if (is_increment_time) {
    mock_initiate_expectation("wifi_timer_interrupt", NULL, 0, NULL);
    mock_initiate_expectation("led_timer_interrupt", NULL, 0, NULL);
  }

  clock_timer_interrupt();

  if (is_increment_time) {
    if((time.min_1 == 0 || time.min_1 == 5) &&
       time.sec_10 == 0 &&
       time.sec_1 == 0) {
      mock_initiate_expectation("wifi_query_timer", NULL, 0, NULL);
    }

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

  set_time(time, is_increment_time);

  clock_main();

  bool is_succeeded = true;
  const time_st *actual_time = clock_get_time();
  TEST_ASSERT(actual_time->hour_10 == time.hour_10, "hour_10");
  TEST_ASSERT(actual_time->hour_1 == time.hour_1, "hour_1");
  TEST_ASSERT(actual_time->min_10 == time.min_10, "min_10");
  TEST_ASSERT(actual_time->min_1 == time.min_1, "min_1");
  TEST_ASSERT(actual_time->sec_10 == time.sec_10, "sec_10");
  TEST_ASSERT(actual_time->sec_1 == time.sec_1, "sec_1");

  return is_succeeded;
}

static void tc_rgbs_all_whole_day(void) {
  bool is_all_suceeded = true;
  for (size_t i = 0; i < ((sizeof(tcs)/sizeof(clock_gen_st)) - 1); i++) {
    TEST_BEGIN();

    clock_gen_st first_time = tcs[i];
    TEST_ASSERT(set_half_second(first_time.time, false, first_time.is_dark_period), "first half");

    log_test("first half - time: %d%d:%d%d:%d%d, is_dark_period: %d",
	     first_time.time.hour_10, first_time.time.hour_1,
	     first_time.time.min_10, first_time.time.min_1,
	     first_time.time.sec_10, first_time.time.sec_1,
	     first_time.is_dark_period);

    clock_gen_st second_time = tcs[i+1];
    TEST_ASSERT(set_half_second(second_time.time, true, second_time.is_dark_period), "second half");

    log_test("second half - time: %d%d:%d%d:%d%d, is_dark_period: %d",
	     second_time.time.hour_10, second_time.time.hour_1,
	     second_time.time.min_10, second_time.time.min_1,
	     second_time.time.sec_10, second_time.time.sec_1,
	     second_time.is_dark_period);

    TEST_END();

    if (!(is_succeeded && is_mock_succeeded)) {
      is_all_suceeded = false;
    }
  }

  if (is_all_suceeded) {
    log_test("All tests succeeded");
  } else {
    log_fail("All tests FAILED!!!!!");
  }
}

int main(void) {
  tc_rgbs_all_whole_day();
  return 0;
}

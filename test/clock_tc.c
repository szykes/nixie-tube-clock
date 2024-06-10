#include "clock.h"

#include <limits.h>

#include "mock.h"
#include "framework.h"
#include "clock_common.h"

#include "led.h"

static bool tc_clock_init(void) {
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
  TEST_ASSERT_EQ(actual_time->hour_10, 2, "hour_10");
  TEST_ASSERT_EQ(actual_time->hour_1, 0, "hour_1");
  TEST_ASSERT_EQ(actual_time->min_10, 0, "min_10");
  TEST_ASSERT_EQ(actual_time->min_1, 0, "min_1");
  TEST_ASSERT_EQ(actual_time->sec_10, 0, "sec_10");
  TEST_ASSERT_EQ(actual_time->sec_1, 1, "sec_1");

  TEST_END();
}

static bool tc_set_get_time(void) {
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
  TEST_ASSERT_EQ(actual_time->hour_10, 2, "hour_10");
  TEST_ASSERT_EQ(actual_time->hour_1, 0, "hour_1");
  TEST_ASSERT_EQ(actual_time->min_10, 5, "min_10");
  TEST_ASSERT_EQ(actual_time->min_1, 9, "min_1");
  TEST_ASSERT_EQ(actual_time->sec_10, 0, "sec_10");
  TEST_ASSERT_EQ(actual_time->sec_1, 0, "sec_1");

  TEST_END();
}

static bool tc_counting_clock_in_dark_period(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, true));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, true));

  TEST_END();
}

static bool tc_counting_clock_in_bright_period(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, false));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, false));

  TEST_END();
}

static bool tc_dark_period_after_midnight(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, true));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, true));

  TEST_END();
}

static bool tc_dark_period_before_min_time(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, true));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, true));

  TEST_END();
}

static bool tc_bright_period_at_min_time(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, false));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, false));

  TEST_END();
}

static bool tc_bright_period_at_noon(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, false));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, false));

  TEST_END();
}

static bool tc_bright_period_at_max_time(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, false));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, false));

  TEST_END();
}

static bool tc_dark_period_after_max_time(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, true));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, true));

  TEST_END();
}

static bool tc_dark_period_before_midnight(void) {
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

  TEST_ASSERT_BOOL(set_half_second(&current_time, expected_time, false, true));
  current_time.sec_1++;
  TEST_ASSERT_BOOL(set_half_second(NULL, current_time, true, true));

  TEST_END();
}

int main(void) {
  TEST_EVALUATE_INIT();
  TEST_EVALUATE(tc_clock_init());
  TEST_EVALUATE(tc_set_get_time());
  TEST_EVALUATE(tc_counting_clock_in_dark_period());
  TEST_EVALUATE(tc_counting_clock_in_bright_period());
  TEST_EVALUATE(tc_dark_period_after_midnight());
  TEST_EVALUATE(tc_dark_period_before_min_time());
  TEST_EVALUATE(tc_bright_period_at_min_time());
  TEST_EVALUATE(tc_bright_period_at_noon());
  TEST_EVALUATE(tc_bright_period_at_max_time());
  TEST_EVALUATE(tc_dark_period_after_max_time());
  TEST_EVALUATE(tc_dark_period_before_midnight());
  TEST_EVALUATE_END();
}

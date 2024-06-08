#include "clock.h"

#include <limits.h>

#include "mock.h"
#include "framework.h"
#include "clock_gen.h"
#include "clock_common.h"

#include "led.h"

static void tc_rgbs_all_whole_day(void) {
  bool is_all_suceeded = true;

  for (size_t i = 0; i < ((sizeof(tcs)/sizeof(clock_gen_st)) - 1); i++) {
    TEST_BEGIN();

    clock_gen_st record = tcs[i];
    TEST_ASSERT_BOOL(set_half_second(NULL, record.time, false, record.is_dark_period));

    log_test("first half - time: %d%d:%d%d:%d%d, is_dark_period: %d",
	     record.time.hour_10, record.time.hour_1,
	     record.time.min_10, record.time.min_1,
	     record.time.sec_10, record.time.sec_1,
	     record.is_dark_period);

    static bool is_first = true;
    if (is_first) {
      // the calculate_time() is not called in the first time in clock.c, so the time_raw_data is empty
      // normally the clock_init() is called at first and it sets properly the time_raw_data
      mock_clear_calls();
      is_first = false;
    }

    clock_gen_st second_time = tcs[i+1];
    TEST_ASSERT_BOOL(set_half_second(NULL, second_time.time, true, second_time.is_dark_period));

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

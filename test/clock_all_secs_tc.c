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

    clock_gen_st first_time = tcs[i];
    TEST_ASSERT_BOOL(set_half_second(NULL, first_time.time, false, first_time.is_dark_period));

    log_test("first half - time: %d%d:%d%d:%d%d, is_dark_period: %d",
	     first_time.time.hour_10, first_time.time.hour_1,
	     first_time.time.min_10, first_time.time.min_1,
	     first_time.time.sec_10, first_time.time.sec_1,
	     first_time.is_dark_period);

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

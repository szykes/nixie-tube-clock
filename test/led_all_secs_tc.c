#include "led.h"

#include "mock.h"
#include "framework.h"
#include "led_gen.h"
#include "led_common.h"

#include "clock.h"

static bool tc_rgbs_all_whole_day(void) {
  for (size_t cnt = 0; cnt < TEST_MAX_CNT; cnt++) {
    mock_initiate_expectation("gpio_led_red_reset", NULL, 0, NULL);
    mock_initiate_expectation("gpio_led_green_reset", NULL, 0, NULL);
    mock_initiate_expectation("gpio_led_blue_reset", NULL, 0, NULL);

    led_isr();
  }

  mock_initiate_expectation("clock_timer_interrupt", NULL, 0, NULL);

  mock_initiate_expectation("gpio_led_red_reset", NULL, 0, NULL);
  mock_initiate_expectation("gpio_led_green_reset", NULL, 0, NULL);
  mock_initiate_expectation("gpio_led_blue_reset", NULL, 0, NULL);

  led_isr();

  bool is_all_suceeded = true;
  for (size_t i = 0; i < (sizeof(tcs)/sizeof(led_gen_st)); i++) {
    TEST_BEGIN();
    set_rgb(tcs[i].time, tcs[i].red_ratio, tcs[i].green_ratio, tcs[i].blue_ratio);
    log_test("time: %d%d:%d%d:%d%d, red_ratio: %d, green_ratio: %d, blue_ratio: %d",
	     tcs[i].time.hour_10, tcs[i].time.hour_1,
	     tcs[i].time.min_10, tcs[i].time.min_1,
	     tcs[i].time.sec_10, tcs[i].time.sec_1,
	     tcs[i].red_ratio,
	     tcs[i].green_ratio,
	     tcs[i].blue_ratio);

    bool is_mock_succeeded = mock_is_succeeded();
    if (is_succeeded && is_mock_succeeded) {
      log_test("Test succeeded");
    } else {
      log_fail("Test FAILED!!!!!");
    }

    if (!(is_succeeded && is_mock_succeeded)) {
      is_all_suceeded = false;
    }
  }

  if (is_all_suceeded) {
    log_test("All tests succeeded");
    return true;
  }
  log_fail("All tests FAILED!!!!!");
  return false;
}

int main(void) {
  return tc_rgbs_all_whole_day() ? 0 : 1;
}

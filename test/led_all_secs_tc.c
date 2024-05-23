#include "led.h"

#include "mock.h"
#include "framework.h"
#include "led_gen.h"

#include "clock.h"

#define TEST_MAX_CNT 240

static void set_rgb(time_st time, uint8_t red_ratio, uint8_t green_ratio, uint8_t blue_ratio) {
  void *param_ptr;
  mock_prepare_param(param_ptr, time);

  type_st ret = {
    .type = TYPE_CONST_TIME_ST_PTR,
    .value = param_ptr,
    .size = sizeof(time),
  };
  mock_initiate_expectation("clock_get_time", NULL, 0, &ret);

  led_timer_interrupt();
  led_main();

  // RGB PWM is NOT set well at first
  for (int cnt = 1; cnt < TEST_MAX_CNT; cnt++) {
    if (cnt >= red_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_red_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }
    if (cnt >= green_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_green_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }
    if (cnt >= blue_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_blue_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }

    led_isr();
    led_main();
  }

 mock_initiate_expectation_with_msg("clock_timer_interrupt", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);

  if (red_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_red_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (green_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_green_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (blue_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_blue_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }

  if (red_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_red_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (green_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_green_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (blue_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_blue_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }

  led_isr();
  led_main();

  // RGB PWM is set finally
  for (int cnt = 1; cnt < TEST_MAX_CNT; cnt++) {
    if (cnt >= red_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_red_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }
    if (cnt >= green_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_green_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }
    if (cnt >= blue_ratio) {
      mock_initiate_expectation_with_msg("gpio_led_blue_reset", NULL, 0, NULL, "in for [%d] r: %d, g: %d, b: %d", cnt, red_ratio, green_ratio, blue_ratio);
    }

    led_isr();
    led_main();
  }

 mock_initiate_expectation_with_msg("clock_timer_interrupt", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);

  if (red_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_red_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (green_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_green_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (blue_ratio != 0) {
   mock_initiate_expectation_with_msg("gpio_led_blue_set", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }

  if (red_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_red_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (green_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_green_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }
  if (blue_ratio == 0) {
   mock_initiate_expectation_with_msg("gpio_led_blue_reset", NULL, 0, NULL, "after for r: %d, g: %d, b: %d", red_ratio, green_ratio, blue_ratio);
  }

  led_isr();
  led_main();
}

static void tc_rgbs_all_whole_day(void) {
  for (int cnt = 0; cnt < TEST_MAX_CNT; cnt++) {
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

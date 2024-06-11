#include "led.h"

#include "mock.h"

#include "led_common.h"

void set_rgb(time_st time, uint8_t red_ratio, uint8_t green_ratio, uint8_t blue_ratio) {
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
  for (size_t cnt = 1; cnt < TEST_MAX_CNT; cnt++) {
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
  for (size_t cnt = 1; cnt < TEST_MAX_CNT; cnt++) {
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

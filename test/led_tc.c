#include "led.h"

#include "mock.h"
#include "framework.h"

#include "clock.h"

#define TEST_MAX_CNT 240
#define TEST_TIMER_INIT_CNT 0xFF

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

static bool tc_led_init(void) {
  TEST_BEGIN();

  unsigned char init_cnt = TEST_TIMER_INIT_CNT;

  void *param_ptr;
  mock_prepare_param(param_ptr, init_cnt);

  type_st params[] = {
    {
      .type = TYPE_UNSIGNED_CHAR,
      .value = param_ptr,
      .size = sizeof(init_cnt),
    }
  };
  mock_initiate_expectation("timer0_init", params, sizeof(params)/sizeof(type_st), NULL);

  led_init();

  TEST_END();
}

static bool tc_led_isr_default(void) {
  TEST_BEGIN();

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

  TEST_END();
}

static bool tc_led_timer_interrupt(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = 0,
      .hour_1 = 0,
      .min_10 = 0,
      .min_1 = 0,
      .sec_10 = 0,
      .sec_1 = 0,
    }, 0, 0, 0);

  set_rgb((time_st) {
      .hour_10 = 1,
      .hour_1 = 1,
      .min_10 = 0,
      .min_1 = 4,
      .sec_10 = 3,
      .sec_1 = 0,
    }, TEST_MAX_CNT, 0, 0);

  TEST_END();
}

static bool tc_rgbs_off_at_after_midnight(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = 0,
      .hour_1 = 0,
      .min_10 = 0,
      .min_1 = 0,
      .sec_10 = 0,
      .sec_1 = 0,
    }, 0, 0, 0);

  TEST_END();
}

static bool tc_rgbs_off_at_min_time(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = MIN_HOUR_10,
      .hour_1 = MIN_HOUR_1,
      .min_10 = MIN_MIN_10,
      .min_1 = 0,
      .sec_10 = 0,
      .sec_1 = 0,
    }, 0, 0, 0);

  TEST_END();
}

static bool tc_rgbs_with_two_colors(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = 0,
      .hour_1 = 6,
      .min_10 = 4,
      .min_1 = 9,
      .sec_10 = 3,
      .sec_1 = 5,
    }, 40, 0, 40);

  TEST_END();
}

static bool tc_rgbs_off_at_max_time(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = MAX_HOUR_10,
      .hour_1 = MAX_HOUR_1,
      .min_10 = MAX_MIN_10,
      .min_1 = 0,
      .sec_10 = 0,
      .sec_1 = 0,
    }, 0, 0, 0);

  TEST_END();
}

static bool tc_rgbs_off_at_before_midnight(void) {
  TEST_BEGIN();

  set_rgb((time_st) {
      .hour_10 = 2,
      .hour_1 = 3,
      .min_10 = 5,
      .min_1 = 9,
      .sec_10 = 5,
      .sec_1 = 9,
    }, 0, 0, 0);

  TEST_END();
}

int main(void) {
  TEST_EVALUATE_INIT();
  TEST_EVALUATE(tc_led_init());
  TEST_EVALUATE(tc_led_isr_default());
  TEST_EVALUATE(tc_led_timer_interrupt());
  TEST_EVALUATE(tc_rgbs_off_at_after_midnight());
  TEST_EVALUATE(tc_rgbs_off_at_min_time());
  TEST_EVALUATE(tc_rgbs_with_two_colors());
  TEST_EVALUATE(tc_rgbs_off_at_max_time());
  TEST_EVALUATE(tc_rgbs_off_at_before_midnight());
  TEST_EVALUATE_END();
}

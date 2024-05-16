#include "led.h"

#include "mock.h"
#include "framework.h"

#define TEST_TIMER_INIT_CNT 0x00

static void tc_led_init(void) {
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

int main(void) {
  tc_led_init();
  return 0;
}

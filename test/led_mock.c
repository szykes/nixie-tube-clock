#include "led.h"

#include "mock.h"

void led_timer_interrupt(void) {
  mock_record(NULL, 0, NULL);
}

void led_is_dark_period(bool is_dark) {
  void *param_ptr;
  mock_prepare_param(param_ptr, is_dark)

  type_st params[] = {
    {
      .type = TYPE_BOOL,
      .value = param_ptr,
    }
  };
  mock_record(params, sizeof(params)/sizeof(type_st), NULL);
}

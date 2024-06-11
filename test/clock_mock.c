#include "clock.h"

#include <string.h>

#include "mock.h"
#include "framework.h"

void clock_timer_interrupt(void) {
  mock_record(NULL, 0, NULL);
}

void clock_update_time(time_st accurate_time) {
  type_st params[] = {
    {
      .type = TYPE_CONST_TIME_ST,
      .value = &accurate_time,
      .size = sizeof(accurate_time),
    }
  };
  mock_record(params, sizeof(params)/sizeof(type_st), NULL);
}

const time_st *clock_get_time(void) {
  type_st ret;

  mock_record(NULL, 0, &ret);

  if (ret.type == TYPE_CONST_TIME_ST_PTR) {
    return (const time_st *)ret.value;
  }

  log_error("Invalid return type");
  return NULL;
}

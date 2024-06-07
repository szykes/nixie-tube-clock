#ifndef TEST_CLOCK_COMMON_H_
#define TEST_CLOCK_COMMON_H_

#include "clock.h"

#include <stdbool.h>

void set_time(time_st time, bool is_glimm);
bool set_half_second(time_st *preset_time, time_st time, bool is_increment_time, bool is_dark_period);

#endif // TEST_CLOCK_COMMON_H_

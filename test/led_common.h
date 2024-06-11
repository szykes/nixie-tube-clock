#ifndef TEST_LED_COMMON_H_
#define TEST_LED_COMMON_H_

#include "clock.h"

#define TEST_MAX_CNT 240
#define TEST_TIMER_INIT_CNT 0xFF

void set_rgb(time_st time, uint8_t red_ratio, uint8_t green_ratio, uint8_t blue_ratio);

#endif // TEST_LED_COMMON_H_

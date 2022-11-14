#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#define MIN_HOUR_10 (0u)
#define MIN_HOUR_1 (6u)
#define MIN_MIN_10 (3u)

#define MAX_HOUR_10 (2u)
#define MAX_HOUR_1 (2u)
#define MAX_MIN_10 (3u)

typedef struct time_st {
  uint8_t hour_10;
  uint8_t hour_1;
  uint8_t min_10;
  uint8_t min_1;
  uint8_t sec_10;
  uint8_t sec_1;
} time_st;

void clock_init(void);

void clock_timer_interrupt(void);

void clock_update_time(time_st accurate_time);
const time_st *clock_get_time(void);

void clock_main(void);

#endif // CLOCK_H_

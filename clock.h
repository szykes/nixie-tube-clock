#ifndef CLOCK_H_
#define CLOCK_H_

typedef struct time_st {
  char hour_10;
  char hour_1;
  char min_10;
  char min_1;
  char sec_10;
  char sec_1;
} time_st;

void clock_init(void);

void clock_timer_interrupt(void);

void clock_update_time(time_st accurate_time);

void clock_main(void);

#endif // CLOCK_H_

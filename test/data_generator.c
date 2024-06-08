#include "led.h"

#include "mock.h"
#include "framework.h"

#include "clock.h"

#include <stdio.h>
#include <math.h>


#define MAX_CNT 240 // 28 800 Hz to 120 Hz

#define HHM_TO_SECS(hour_10, hour_1, min_10) (		\
    (hour_10) * 10 * 3600 +				\
    (hour_1) * 3600 +					\
    (min_10) * 10 * 60)

#define HHMMSS_TO_SECS(hour_10, hour_1, min_10, min_1,	\
		       sec_10, sec_1) (			\
    (hour_10) * 10 * 3600 +				\
    (hour_1) * 3600 +					\
    (min_10) * 10 * 60)	+				\
    (min_1) * 60 +					\
    (sec_10) * 10 +					\
    (sec_1)

#define SECONDS_OF_DAY (HHMMSS_TO_SECS(2, 3, 5, 9, 5, 9) + 1)

#define BRIGHT_PERIOD_IN_SECS ( \
  HHM_TO_SECS(MAX_HOUR_10, MAX_HOUR_1, MAX_MIN_10) - \
  HHM_TO_SECS(MIN_HOUR_10, MIN_HOUR_1, MIN_MIN_10))

#define SEVENTH_OF_BRIGHT_PERIOD_IN_SECS (BRIGHT_PERIOD_IN_SECS / (float) 7)

#define RATIO_MODIFIER 0

static uint8_t increasing_ratio(uint32_t base) {
  uint16_t ratio = lrintf(base / (SEVENTH_OF_BRIGHT_PERIOD_IN_SECS / (float) (MAX_CNT + RATIO_MODIFIER)));
  return (ratio > MAX_CNT ? MAX_CNT : ratio);
}

static uint8_t decreasing_ratio(uint32_t base) {
  uint16_t ratio = lrintf(base / (SEVENTH_OF_BRIGHT_PERIOD_IN_SECS / (float) (MAX_CNT + RATIO_MODIFIER)));
  return MAX_CNT - (ratio > MAX_CNT ? MAX_CNT : ratio);
}

static time_st convert_seconds_to_time_st(uint32_t secs) {
  secs %= SECONDS_OF_DAY;

  time_st time;
  time.hour_10 = secs / 36000;
  secs %= 36000;
  time.hour_1 = secs / 3600;
  secs %= 3600;
  time.min_10 = secs / 600;
  secs %= 600;
  time.min_1 = secs / 60;
  secs %= 60;
  time.sec_10 = secs / 10;
  secs %= 10;
  time.sec_1 = secs;
  return time;
}

static void fill_led_gen_h(void) {
  FILE *fp = fopen("test/led_gen.h", "w+");

  fprintf(fp, "// DO NOT CHANGE THIS FILE! This file is generated.\n");
  fprintf(fp, "#ifndef TEST_LED_GEN_H_\n");
  fprintf(fp, "#define TEST_LED_GEN_H_\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include <stdint.h>\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include \"clock.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "typedef struct {\n");
  fprintf(fp, "  time_st time;\n");
  fprintf(fp, "  uint8_t red_ratio;\n");
  fprintf(fp, "  uint8_t green_ratio;\n");
  fprintf(fp, "  uint8_t blue_ratio;\n");
  fprintf(fp, "} led_gen_st;\n");
  fprintf(fp, "\n");
  fprintf(fp, "extern led_gen_st tcs[%d];\n", SECONDS_OF_DAY);
  fprintf(fp, "\n");
  fprintf(fp, "#endif // TEST_LED_GEN_H_\n");

  fflush(fp);

  fclose(fp);
}

static void fill_led_gen_c(void) {
  FILE *fp = fopen("test/led_gen.c", "w+");

  fprintf(fp, "// DO NOT CHANGE THIS FILE! This file is generated.\n");
  fprintf(fp, "#include \"clock.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include \"led_gen.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "// size of part: %f\n", SEVENTH_OF_BRIGHT_PERIOD_IN_SECS);
  fprintf(fp, "\n");
  fprintf(fp, "led_gen_st tcs[%d] = {\n", SECONDS_OF_DAY);

  for (int64_t secs = 0; secs < SECONDS_OF_DAY; secs++) {
    uint8_t red_ratio;
    uint8_t green_ratio;
    uint8_t blue_ratio;

    const char *part = NULL;
    int64_t sec = secs - HHM_TO_SECS(MIN_HOUR_10, MIN_HOUR_1, MIN_MIN_10);

    if (sec < 0) {
      sec = 0;
    }

    uint32_t base = sec % ((uint32_t) SEVENTH_OF_BRIGHT_PERIOD_IN_SECS + 1);

    if (sec > (7 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "8th part";
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = 0;
    } else if (sec > (6 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "7th part";
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = decreasing_ratio(base);
    } else if (sec > (5 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "6th part";
      red_ratio = 0;
      green_ratio = decreasing_ratio(base);
      blue_ratio = MAX_CNT;
    } else if (sec > (4 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "5th part";
      red_ratio = 0;
      green_ratio = MAX_CNT;
      blue_ratio = increasing_ratio(base);
    } else if (sec > (3 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "4th part";
      red_ratio = decreasing_ratio(base);
      green_ratio = MAX_CNT;
      blue_ratio = 0;
    } else if (sec > (2 * SEVENTH_OF_BRIGHT_PERIOD_IN_SECS)) {
      part = "3rd part";
      red_ratio = MAX_CNT;
      green_ratio = increasing_ratio(base);
      blue_ratio = 0;
    } else if (sec > SEVENTH_OF_BRIGHT_PERIOD_IN_SECS) {
      part = "2nd part";
      red_ratio = MAX_CNT;
      green_ratio = 0;
      blue_ratio = decreasing_ratio(base);
    } else if (sec > 0) {
      part = "1st part";
      red_ratio = increasing_ratio(base);
      green_ratio = 0;
      blue_ratio = increasing_ratio(base);
    } else {
      part = "out of part";
      red_ratio = 0;
      green_ratio = 0;
      blue_ratio = 0;
    }

    time_st time = convert_seconds_to_time_st(secs);

    fprintf(fp, "  {.time = {.hour_10 = %d, .hour_1 = %d, .min_10 = %d, .min_1 = %d, .sec_10 = %d, .sec_1 = %d}, .red_ratio = %d, .green_ratio = %d, .blue_ratio = %d},\t// secs=%ld, time=%d%d:%d%d:%d%d, part='%s'\n",
	    time.hour_10, time.hour_1,
	    time.min_10, time.min_1,
	    time.sec_10, time.sec_1,
	    red_ratio, green_ratio, blue_ratio,
	    secs,
	    time.hour_10, time.hour_1,
	    time.min_10, time.min_1,
	    time.sec_10, time.sec_1,
	    part);
  }

  fprintf(fp, "};\n");

  fflush(fp);

  fclose(fp);
}

static void fill_clock_gen_h(void) {
  FILE *fp = fopen("test/clock_gen.h", "w+");

  fprintf(fp, "// DO NOT CHANGE THIS FILE! This file is generated.\n");
  fprintf(fp, "#ifndef TEST_CLOCK_GEN_H_\n");
  fprintf(fp, "#define TEST_CLOCK_GEN_H_\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include <stdbool.h>\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include \"clock.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "typedef struct {\n");
  fprintf(fp, "  time_st time;\n");
  fprintf(fp, "  bool is_dark_period;\n");
  fprintf(fp, "} clock_gen_st;\n");
  fprintf(fp, "\n");
  fprintf(fp, "extern clock_gen_st tcs[%d];\n", (SECONDS_OF_DAY + 1));
  fprintf(fp, "\n");
  fprintf(fp, "#endif // TEST_CLOCK_GEN_H_\n");

  fflush(fp);

  fclose(fp);
}

static void fill_clock_gen_c(void) {
  FILE *fp = fopen("test/clock_gen.c", "w+");

  fprintf(fp, "// DO NOT CHANGE THIS FILE! This file is generated.\n");
  fprintf(fp, "#include \"clock.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "#include \"clock_gen.h\"\n");
  fprintf(fp, "\n");
  fprintf(fp, "// size of part: %f\n", SEVENTH_OF_BRIGHT_PERIOD_IN_SECS);
  fprintf(fp, "\n");
  fprintf(fp, "clock_gen_st tcs[%d] = {\n", (SECONDS_OF_DAY + 1));

  for (uint32_t secs = 0; secs < (SECONDS_OF_DAY + 1); secs++) {
    bool is_dark_period = true;

    if (HHM_TO_SECS(MIN_HOUR_10, MIN_HOUR_1, MIN_MIN_10) <= secs && secs < HHM_TO_SECS(MAX_HOUR_10, MAX_HOUR_1, MAX_MIN_10)) {
      is_dark_period = false;
    }

    time_st time = convert_seconds_to_time_st(secs);

    fprintf(fp, "  {.time = {.hour_10 = %d, .hour_1 = %d, .min_10 = %d, .min_1 = %d, .sec_10 = %d, .sec_1 = %d}, .is_dark_period = %s},\t// secs=%u, time=%d%d:%d%d:%d%d\n",
	    time.hour_10, time.hour_1,
	    time.min_10, time.min_1,
	    time.sec_10, time.sec_1,
      	    (is_dark_period ? "true" : "false"),
	    secs,
	    time.hour_10, time.hour_1,
	    time.min_10, time.min_1,
	    time.sec_10, time.sec_1);
  }

  fprintf(fp, "};\n");

  fflush(fp);

  fclose(fp);
}

int main(void) {
  fill_led_gen_h();
  fill_led_gen_c();
  fill_clock_gen_h();
  fill_clock_gen_c();
  return 0;
}

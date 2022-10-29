#ifndef LED_H
#define LED_H

#include <stdbool.h>

#define ONE_SEC_CNT 120

void led_init(void);

void led_set_red_ratio(char ratio);
void led_set_green_ratio(char ratio);
void led_set_blue_ratio(char ratio);
void led_is_dark_period(bool is_dark);

#endif // LED_H

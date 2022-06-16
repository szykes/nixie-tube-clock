#ifndef WIFI_H_
#define WIFI_H_

void wifi_init(void);

void wifi_timer_interrupt(void);

char wifi_get_led_red_ratio(void);
char wifi_get_led_green_ratio(void);
char wifi_get_led_blue_ratio(void);

void wifi_main(void);

#endif // WIFI_H_

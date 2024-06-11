#ifndef WIFI_H_
#define WIFI_H_

#include <stdint.h>

void wifi_receive_data(uint8_t data);

void wifi_init(void);

void wifi_timer_interrupt(void);

void wifi_main(void);

void wifi_query_timer(void);

#endif // WIFI_H_

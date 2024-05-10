#include "wifi.h"

#include "mock.h"

void wifi_timer_interrupt(void) {
  mock_record(NULL, 0, NULL);
}

void wifi_query_timer(void) {
  mock_record(NULL, 0, NULL);
}

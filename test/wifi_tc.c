#include "wifi.h"

#include "mock.h"
#include "framework.h"

static void tc_wifi_query_timer(void) {
  TEST_BEGIN();

  mock_expect("gpio_set_ch_pd", NULL, 0, NULL);

  wifi_query_timer();

  TEST_END();
}

int main(void) {
  tc_wifi_query_timer();
  return 0;
}

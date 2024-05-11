#include "wifi.h"

#include "mock.h"
#include "framework.h"

#include "clock.h"

static void set_wifi_init(void) {
  mock_expect("uart_init", NULL, 0, NULL);
  mock_expect("gpio_set_ch_pd", NULL, 0, NULL);
  mock_expect("gpio_esp_set", NULL, 0, NULL);

  wifi_init();
}

static void set_esp_timer_default_timeout(void) {
  for (int i = 0; i < 2; i++) {
    wifi_main();
    wifi_timer_interrupt();
  }

  mock_expect("gpio_esp_reset", NULL, 0, NULL);
  mock_expect("gpio_esp_set", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();
}

static void set_esp_timer_long_timeout(void) {
  for (int i = 0; i < 10; i++) {
    wifi_main();
    wifi_timer_interrupt();
  }

  mock_expect("gpio_esp_reset", NULL, 0, NULL);
  mock_expect("gpio_esp_set", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();
}

static void mock_uart_send(const char* data) {
  for (int i = 0; i < strlen(data); i++) {
    void *param_ptr;
    mock_prepare_param(param_ptr, data[i]);

    type_st params[] = {
      {
	.type = TYPE_CHAR,
	.value = param_ptr,
      }
    };
    mock_expect("uart_send_data", params, sizeof(params)/sizeof(type_st), NULL);
  }
}

static void set_wifi_receive_data(const char *data) {
  for (int i = 0; i < strlen(data); i++) {
    wifi_receive_data(data[i]);
  }
}

static void reach_send_alive_check(const char *str) {
  for (int i = 0; i < strlen(str); i++) {
    wifi_main();
    wifi_receive_data(str[i]);
  }

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT\r\n");

  wifi_main();
}

static void reach_set_wifi_mode(void) {
  reach_send_alive_check("ready\n");

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT+CWMODE=1\r\n");

  wifi_main();
}

static void reach_connect_to_ap(void) {
  reach_set_wifi_mode();

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWD "\"\r\n");

  wifi_main();
}

static void reach_set_multiple_connections_mode(void) {
  reach_connect_to_ap();

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT+CIPMUX=1\r\n");

  wifi_main();
}

static void reach_create_tcp_server(void) {
  reach_set_multiple_connections_mode();

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT+CIPSERVER=1," ESP_TCP_PORT "\r\n");

  wifi_main();
}

static void reach_establish_tcp_connection(void) {
  reach_create_tcp_server();

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  mock_uart_send("AT+CIPSTART=0,\"TCP\",\"" TIME_SERVER_IP "\"," TIME_SERVER_PORT "\r\n");

  wifi_main();
}

static void tc_wifi_init(void) {
  TEST_BEGIN();

  set_wifi_init();

  TEST_END();
}

static void tc_esp_timer_long_timeout(void) {
  TEST_BEGIN();

  set_wifi_init();

  set_esp_timer_long_timeout();

  // no further action should be taken
  for (int i = 0; i < 20; i++) {
    wifi_timer_interrupt();
    wifi_main();
  }

  TEST_END();
}

static void tc_garbage_data_received(void) {
  TEST_BEGIN();

  set_wifi_receive_data("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F");
  set_wifi_receive_data("\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F");
  set_wifi_receive_data(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`");
  set_wifi_receive_data("abcdefghijklmnopqrstuvwxyz{|}~\x7F");
  set_wifi_receive_data("\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F");
  set_wifi_receive_data("\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F");
  set_wifi_receive_data("\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF");
  set_wifi_receive_data("\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF");
  set_wifi_receive_data("\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF");
  set_wifi_receive_data("\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF");
  set_wifi_receive_data("\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF");
  set_wifi_receive_data("\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  wifi_main();

  set_wifi_receive_data("\r\n");

  wifi_main();

  TEST_END();
}

static void tc_frag_garbage_data_received(void) {
  TEST_BEGIN();

  const char* strs[] = {
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\r\n",
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\r\n",
    " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`\r\n",
    "abcdefghijklmnopqrstuvwxyz{|}~\x7F\r\n",
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\r\n",
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\r\n",
    "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\r\n",
    "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF\r\n",
    "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\r\n",
    "\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\r\n",
    "\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\r\n",
    "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF\r\n",
  };

  for (int i = 0; i < sizeof(strs)/sizeof(char*); i++) {
    set_wifi_receive_data(strs[i]);

    mock_expect("mcu_cli", NULL, 0, NULL);
    mock_expect("mcu_sei", NULL, 0, NULL);

    wifi_main();
  }

  set_wifi_receive_data("\r\n");

  wifi_main();

  TEST_END();
}

static void tc_send_alive_check_with_ready(void) {
  TEST_BEGIN();

  reach_send_alive_check("ready\n");

  TEST_END();
}

static void tc_send_alive_check_with_invalid(void) {
  TEST_BEGIN();

  set_wifi_init();

  reach_send_alive_check("inval\n");

  TEST_END();
}

static void tc_alive_check_failed(void) {
  TEST_BEGIN();

  reach_send_alive_check("ready\n");

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_alive_check_esp_timeout(void) {
  TEST_BEGIN();

  reach_send_alive_check("ready\n");

  set_esp_timer_default_timeout();

  TEST_END();
}

static void tc_set_wifi_mode_failed(void) {
  TEST_BEGIN();

  reach_set_wifi_mode();

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_set_wifi_mode_esp_timeout(void) {
  TEST_BEGIN();

  reach_set_wifi_mode();

  set_esp_timer_default_timeout();

  TEST_END();
}

static void tc_connect_to_ap_failed(void) {
  TEST_BEGIN();

  reach_connect_to_ap();

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_connect_to_ap_esp_timeout(void) {
  TEST_BEGIN();

  reach_connect_to_ap();

  set_esp_timer_long_timeout();

  TEST_END();
}

static void tc_set_multiple_connections_mode_failed(void) {
  TEST_BEGIN();

  reach_set_multiple_connections_mode();

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_set_multiple_connections_mode_esp_timeout(void) {
  TEST_BEGIN();

  reach_set_multiple_connections_mode();

  set_esp_timer_default_timeout();

  TEST_END();
}

static void tc_create_tcp_server_failed(void) {
  TEST_BEGIN();

  reach_create_tcp_server();

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_create_tcp_server_esp_timeout(void) {
  TEST_BEGIN();

  reach_create_tcp_server();

  set_esp_timer_default_timeout();

  TEST_END();
}

static void tc_establish_tcp_connection_failed(void) {
  TEST_BEGIN();

  reach_establish_tcp_connection();

  set_wifi_receive_data("ERROR\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);
  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_establish_tcp_connection_esp_timeout(void) {
  TEST_BEGIN();

  reach_establish_tcp_connection();

  set_esp_timer_default_timeout();

  TEST_END();
}

static void tc_establish_tcp_connection_succeed(void) {
  TEST_BEGIN();

  reach_establish_tcp_connection();

  set_wifi_receive_data("OK\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  wifi_main();

  for (int i = 0; i < 10; i++) {
    wifi_main();
    wifi_timer_interrupt();
  }

  wifi_main();

  set_wifi_receive_data("+IPD,0,x:1234560,CLOSED\r\n");

  mock_expect("mcu_cli", NULL, 0, NULL);
  mock_expect("mcu_sei", NULL, 0, NULL);

  time_st time;
  time.hour_10 = 1;
  time.hour_1 = 2;
  time.min_10 = 3;
  time.min_1 = 4;
  time.sec_10 = 5;
  time.sec_1 = 6;

  void *param_ptr;
  mock_prepare_param(param_ptr, time);

  type_st params[] = {
    {
      .type = TYPE_CONST_TIME_ST,
      .value = param_ptr,
    }
  };
  mock_expect("clock_update_time", params, sizeof(params)/sizeof(type_st), NULL);

  mock_expect("gpio_reset_ch_pd", NULL, 0, NULL);

  wifi_main();

  TEST_END();
}

static void tc_wifi_query_timer(void) {
  TEST_BEGIN();

  mock_expect("gpio_set_ch_pd", NULL, 0, NULL);

  wifi_query_timer();

  TEST_END();
}

int main(void) {
  tc_wifi_init();
  tc_esp_timer_long_timeout();
  tc_garbage_data_received();
  tc_frag_garbage_data_received();
  tc_send_alive_check_with_ready();
  tc_send_alive_check_with_invalid();
  tc_alive_check_failed();
  tc_alive_check_esp_timeout();
  tc_set_wifi_mode_failed();
  tc_set_wifi_mode_esp_timeout();
  tc_connect_to_ap_failed();
  tc_connect_to_ap_esp_timeout();
  tc_set_multiple_connections_mode_failed();
  tc_set_multiple_connections_mode_esp_timeout();
  tc_create_tcp_server_failed();
  tc_create_tcp_server_esp_timeout();
  tc_establish_tcp_connection_failed();
  tc_establish_tcp_connection_esp_timeout();
  tc_establish_tcp_connection_succeed();
  tc_wifi_query_timer();
  return 0;
}

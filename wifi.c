#include "wifi.h"
#include "clock.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#define BAUD 115200
#include <util/setbaud.h>

#include "gpio.h"

#define AT_CMD_TIMER_STOP    0
#define AT_CMD_TIMER_DEFAULT 2
#define AT_CMD_TIMER_LONG    10

typedef enum {
  AT_CMD_TYPE_NONE = 0,
  AT_CMD_TYPE_AT,            // AT
  AT_CMD_TYPE_SET_CWMODE,    // AT+CWMODE=
  AT_CMD_TYPE_SET_CWJAP,     // AT+CWJAP=
  AT_CMD_TYPE_SET_CIPMUX,    // AT+CIPMUX=
  AT_CMD_TYPE_SET_CIPSERVER, // AT+CIPSERVER=
  AT_CMD_TYPE_SET_CIPSTART,   // AT+CIPSTAR=
} at_cmd_type;

typedef void (*at_cmd_response_handler_func)(const char *buf, size_t len);

static volatile char recv_buffer[32];
static volatile size_t recv_buffer_len;

static volatile int8_t esp_timer = -1;
static volatile bool esp_reset = false;

static at_cmd_type sent_at_cmd = AT_CMD_TYPE_NONE;

static void esp_timer_start(int8_t timeout_sec) {
  esp_timer = timeout_sec;
}

static void esp_timer_stop(void) {
  esp_timer = AT_CMD_TIMER_STOP;
}

static void esp_timer_counter(void) {
  if (esp_timer > AT_CMD_TIMER_STOP) {
    esp_timer--;
    if (esp_timer <= AT_CMD_TIMER_STOP) {
      esp_reset = true;
    }
  }
}

static void send_data_exec(char data) {
  while (!(UCSRA & (1 << UDRE)));
  UDR = data;
}

static void send_data(const char *data, size_t len) {
  size_t i = 0;

  for(i = 0; i < len && data[i] != '\0'; i++) {
    send_data_exec(data[i]);
  }

  if(len >= 2 && data[0] == 'A' && data[1] == 'T') {
    send_data_exec('\r');
    send_data_exec('\n');
  }
}

static void send_alive_check(void) {
  const char cmd[] = "AT";
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_AT;

  esp_timer_start(AT_CMD_TIMER_DEFAULT);
}

static void send_set_wifi_mode(void) {
  const char cmd[] = "AT+CWMODE=1"; // set Station mode
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_SET_CWMODE;

  esp_timer_start(AT_CMD_TIMER_DEFAULT);
}

static void send_connect_to_ap(void) {
  const char cmd[] = "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWD "\""; // connect to AP
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_SET_CWJAP;

  esp_timer_start(AT_CMD_TIMER_LONG);
}

static void send_set_multiple_connections_mode(void) {
  const char cmd[] = "AT+CIPMUX=1"; // set multiple connections mode
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_SET_CIPMUX;

  esp_timer_start(AT_CMD_TIMER_DEFAULT);
}

static void send_create_tcp_server(void) {
  const char cmd[] = "AT+CIPSERVER=1," ESP_TCP_PORT; // create server
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_SET_CIPSERVER;

  esp_timer_start(AT_CMD_TIMER_DEFAULT);
}

static void send_establish_tcp_connection(void) {
  const char cmd[] = "AT+CIPSTART=0,\"TCP\",\"" TIME_SERVER_IP "\"," TIME_SERVER_PORT;
  send_data(cmd, sizeof(cmd));

  sent_at_cmd = AT_CMD_TYPE_SET_CIPSTART;

  esp_timer_start(AT_CMD_TIMER_DEFAULT);
}

static void response_handler_alive_check(const char *buf, size_t len) {
  if(buf[0] == 'O') { // response OK
    esp_timer_stop();

    send_set_wifi_mode();
  }
}

static void response_handler_set_wifi_mode(const char *buf, size_t len) {
  if (buf[0] == 'O') { // response OK
    esp_timer_stop();

    send_connect_to_ap();
  }
}

static void response_handler_connect_to_ap(const char *buf, size_t len) {
  if (buf[0] == 'O') { // response OK
    esp_timer_stop();

    send_set_multiple_connections_mode();
  }
}

static void response_handler_set_multiple_connections_mode(const char *buf, size_t len) {
  if (buf[0] == 'O') { // response OK
    esp_timer_stop();

    send_create_tcp_server();
  }
}

static void response_handler_create_tcp_server(const char *buf, size_t len) {
  if (buf[0] == 'O') { // response OK
    esp_timer_stop();

    send_establish_tcp_connection();
  }
}

static void response_handler_establish_tcp_connection(const char *buf, size_t len) {
  if (buf[0] == 'O') { // response OK
    esp_timer_stop();
  } else if (buf[0] == '+' && buf[1] == 'I' && buf[2] == 'P' && buf[3] == 'D') {
    // example event: +IPD,0,6:1346390,CLOSED

    time_st accurate_time;
    accurate_time.hour_10 = buf[9] - '0';
    accurate_time.hour_1 = buf[10] - '0';
    accurate_time.min_10 = buf[11] - '0';
    accurate_time.min_1 = buf[12] - '0';
    accurate_time.sec_10 = buf[13] - '0';
    accurate_time.sec_1 = buf[14] - '0';
    clock_update_time(accurate_time);

    gpio_reset_ch_pd();
  }
}

static at_cmd_response_handler_func at_cmd_response_handler[] = {
  /* AT_CMD_TYPE_NONE          */ NULL,
  /* AT_CMD_TYPE_AT            */ response_handler_alive_check,
  /* AT_CMD_TYPE_SET_CWMODE    */ response_handler_set_wifi_mode,
  /* AT_CMD_TYPE_SET_CWJAP     */ response_handler_connect_to_ap,
  /* AT_CMD_TYPE_SET_CIPMUX    */ response_handler_set_multiple_connections_mode,
  /* AT_CMD_TYPE_SET_CIPSERVER */ response_handler_create_tcp_server,
  /* AT_CMD_TYPE_SET_CIPSTART  */ response_handler_establish_tcp_connection,
};

static void parse_response(const char *buf, size_t len) {
  // some ESP01 response with 'ready' and some with 'invalid', when ESP is ready
  if((buf[0] == 'r' && buf[1] == 'e' && buf[2] == 'a' && buf[3] == 'd' && buf[4] == 'y') ||
     (buf[0] == 'i' && buf[1] == 'n' && buf[2] == 'v' && buf[3] == 'a' && buf[4] == 'l')) {
    esp_timer_stop();
    send_alive_check();
    return;
  }

  if (sent_at_cmd != AT_CMD_TYPE_NONE) {
    (*at_cmd_response_handler[sent_at_cmd])(buf, len);
  }
}

static void read_recv_buffer(void) {
  char buf[sizeof(recv_buffer)];
  size_t len = 0;

  cli();

  if(recv_buffer_len > 0) {
    size_t i;

    for(i = 0; i < recv_buffer_len; i++) {
      buf[i] = recv_buffer[i];
    }

    len = recv_buffer_len;
  }

  sei();

  if(len == 0) {
    return;
  }

  recv_buffer_len = 0;

  parse_response(buf, len);
}

ISR(USART_RXC_vect) {
  static char buf[sizeof(recv_buffer)];
  static size_t idx;

  char temp = UDR;

  if(idx < (sizeof(recv_buffer) - 1)) {
    buf[idx] = temp;
    idx++;
  }

  if(temp == '\n') {
    size_t i;

    buf[idx] = '\0';

    for(i = 0; i < (idx + 1); i++) {
      recv_buffer[i] = buf[i];
    }

    recv_buffer_len = idx;
    idx = 0;
  }
}

void wifi_init(void) {
  UBRRH = UBRRH_VALUE;
  UBRRL = UBRRL_VALUE;
#if USE_2X
  UCSRA |= (1 << U2X);
#else
  UCSRA &= ~(1 << U2X);
#endif

  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
  UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);

  gpio_set_ch_pd();
  gpio_esp_set();

  esp_timer_start(AT_CMD_TIMER_LONG);
}

void wifi_timer_interrupt(void) {
  esp_timer_counter();
}

void wifi_main(void) {
  if(esp_reset) {
    gpio_esp_reset();
    gpio_esp_set();
    gpio_reset_ch_pd();
    gpio_set_ch_pd();
    esp_reset = false;
  }

  if (recv_buffer_len != 0) {
    read_recv_buffer();
  }
}

void wifi_query_timer(void) {
  gpio_set_ch_pd();
}

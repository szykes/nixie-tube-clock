#include "wifi.h"

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

#define AT_CMD_TIMER_STOP (-1)
#define AT_CMD_TIMER_TIMEOUT 5

typedef enum {
  NONE,
  AT,            // AT
  SET_CWMODE,    // AT+CWMODE=
  SET_CWJAP,     // AT+CWJAP=
  SET_CWQAP,     // AT+CWQAP
  SET_CIPMUX,    // AT+CIPMUX=
  SET_CIPSERVER, // AT+CIPSERVER=
} at_cmd_type;

static volatile char red_ratio = 10;
static volatile char green_ratio = 150;
static volatile char blue_ratio = 120;

static volatile char recv_buffer[32];
static volatile size_t recv_buffer_len;

static volatile char at_cmd_timer = -1;
static volatile bool esp_reset = false;

static at_cmd_type sent_at_cmd = NONE;

static void at_cmd_response_timer_start(char timeout_sec) {
  at_cmd_timer = timeout_sec;
}

static void at_cmd_response_timer_stop(void) {
  at_cmd_timer = AT_CMD_TIMER_STOP;
}

static void at_cmd_response_timer_counter(void) {
  if(at_cmd_timer >= 0) {
    at_cmd_timer--;
    if(at_cmd_timer <= 0) {
      at_cmd_timer = AT_CMD_TIMER_STOP;
      esp_reset = true;
    }
  }
}

static void send_data_exec(char data) {
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = data;
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

  at_cmd_response_timer_start();
}

static void parse_response(const char *buf, size_t len) {
  switch (buf[0]) {
  case 'O': // assumed response: OK
    break;
  case 'E': // assumed response: ERROR
    break;
  case 'W': // assumed response: WIFI*
      if (recv_buffer_len >= 2 && recv_buffer[0] == 'W' && recv_buffer[1] == 'I') {
    cli();
    recv_buffer_len = 0;
    sei();
    const char at[] = "AT";
    send_data(at, sizeof(at));
  }
    break;
  case 'C': // assumed response: CONNECT
    break;
  case '+': // assumed response: +*
    break;
  default:
    break;
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

  at_cmd_response_timer_stop();

  parse_response(buf, len);
}

ISR(USART_RX_vect) {
  static char buf[sizeof(recv_buffer)];
  static size_t idx;

  char temp = UDR0;

  if(idx < (sizeof(recv_buffer) - 1)) {
    buf[idx] = temp;
  }

  idx++;

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
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  gpio_esp_reset();

  // const char at[] = "AT";
  //send_data(at, sizeof(at));

  //_delay_ms(100);

#if 0
  const char atgmr[] = "AT+GMR\r\n";
  send_data(atgmr, sizeof(atgmr));

  _delay_ms(100);
#endif

#if 0
  const char atciobaud[] = "AT+CIOBAUD?\r\n";
  send_data(atciobaud, sizeof(atciobaud));

  _delay_ms(100);
#endif

#if 0
  const char atcwmodeset[] = "AT+CWMODE=1\r\n";
  send_data(atcwmodeset, sizeof(atcwmodeset));

  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
#endif

#if 0
  const char atcwmode[] = "AT+CWMODE?\r\n";
  send_data(atcwmode, sizeof(atcwmode));

  _delay_ms(100);
#endif

#if 0
  const char atcwlap[] = "AT+CWLAP\r\n";
  send_data(atcwlap, sizeof(atcwlap));

  _delay_ms(100);
#endif

#if 0
  const char atcwjap[] = "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWD "\"\r\n";
  send_data(atcwjap, sizeof(atcwjap));

  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
  _delay_ms(100);
#endif

#if 0
  const char atcwqap[] = "AT+CWQAP\r\n";
  send_data(atcwqap, sizeof(atcwqap));

  _delay_ms(100);
#endif

#if 0
  const char atmac[] = "AT+CIPSTAMAC?\r\n";
  send_data(atmac, sizeof(atmac));

  _delay_ms(100);
#endif

#if 0
  const char atcwhost[] = "AT+CWHOSTNAME=\"my_test\"\r\n";
  send_data(atcwhost, sizeof(atcwhost));

  _delay_ms(100);
#endif

#if 0
  const char atcipsta[] = "AT+CIPSTA?\r\n";
  send_data(atcipsta, sizeof(atcipsta));

  _delay_ms(100);
#endif

#if 0
  const char atcipmux[] = "AT+CIPMUX=1\r\n";
  send_data(atcipmux, sizeof(atcipmux));

  _delay_ms(100);
#endif

#if 0
  const char atcipserver[] = "AT+CIPSERVER=1,80\r\n";
  send_data(atcipserver, sizeof(atcipserver));

  _delay_ms(100);
#endif

#if 0
  const char atcipsend[] = "AT+CIPSEND=0,5\r\n";
  send_data(atcipsend, sizeof(atcipsend));

  _delay_ms(100);

  const char data[] = "12345";
  send_data(data, sizeof(data) - 1);

  _delay_ms(100);
#endif

}

void wifi_timer_interrupt(void) { at_cmd_response_timer_counter(); }

char wifi_get_led_red_ratio(void) {
  return red_ratio;
}

char wifi_get_led_green_ratio(void) {
  return green_ratio;
}

char wifi_get_led_blue_ratio(void) {
  return blue_ratio;
}

void wifi_main(void) {
  if(esp_reset) {
    gpio_esp_reset();
    esp_reset = false;
  }

  static int once;
  if(!once) {
    const char at[] = "AT";
    send_data(at, sizeof(at));
    once = 1;
  }

  if (recv_buffer_len != 0) {
    read_recv_buffer();
  }
}

#if 0
 char data[2][26]={
    "OK", "+IPD:0,15,20220425T183150"
  };

  size_t i;
  char* ipd="+IPD:";
  char found = 1;
  for(i=0;i<sizeof(*ipd)-1;i++){
    if(ipd[i] != data[1][i]){
      found = 0;
    }
  }

  if(!found){
    return 0;
  }

  size_t h, m;
      char buff[3];
  for(i=19;i<21;i++){
    buff[i-19] = data[1][i];
  }
  buff[2] = 0;
    h = atoi(buff);

  for(;i<23;i++){
    buff[i-21] = data[1][i];
  }
  m = atoi(buff);

  if (m == 1){
    _delay_ms(1);
  }

    while(1) // Infinite loop
    {
      _delay_ms(500);  // Delay for 500 ms
      tbi(PORTB, (h+m)); // the toggling takes place here
    }

    return 0;
}
#endif

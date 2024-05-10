#include "avr.h"

#include "mock.h"

void mcu_sei(void) {
  mock_record(NULL, 0, NULL);
}

void mcu_cli(void) {
  mock_record(NULL, 0, NULL);
}

void uart_init(void) {
  mock_record(NULL, 0, NULL);
}

void uart_send_data(char data) {
  void *param_ptr;
  mock_prepare_param(param_ptr, data);

  type_st params[] = {
    {
      .type = TYPE_UNSIGNED_CHAR,
      .value = param_ptr,
    }
  };
  mock_record(params, sizeof(params)/sizeof(type_st), NULL);
}

void timer0_init(unsigned char cnt) {
  void *param_ptr;
  mock_prepare_param(param_ptr, cnt);

  type_st params[] = {
    {
      .type = TYPE_CHAR,
      .value = param_ptr,
    }
  };
  mock_record(params, sizeof(params)/sizeof(type_st), NULL);
}

void gpio_init(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_red_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_red_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_green_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_green_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_blue_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_led_blue_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_data_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_data_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_do_clk_cyc(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_latch_enable_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_latch_enable_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_blanking_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_blanking_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_polarity_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_polarity_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_esp_set(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_esp_reset(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_set_ch_pd(void) {
  mock_record(NULL, 0, NULL);
}

void gpio_reset_ch_pd(void) {
  mock_record(NULL, 0, NULL);
}

void wdt_init(void) {
  mock_record(NULL, 0, NULL);
}

void wdt_restart(void) {
  mock_record(NULL, 0, NULL);
}

#ifndef AVR_H_
#define AVR_H_

void mcu_sei(void);
void mcu_cli(void);

void uart_init(void);
void uart_send_data(char data);

void timer0_init(unsigned char cnt);

void gpio_init(void);
void gpio_led_red_set(void);
void gpio_led_red_reset(void);
void gpio_led_green_set(void);
void gpio_led_green_reset(void);
void gpio_led_blue_set(void);
void gpio_led_blue_reset(void);
void gpio_data_set(void);
void gpio_data_reset(void);
void gpio_do_clk_cyc(void);
void gpio_latch_enable_set(void);
void gpio_latch_enable_reset(void);
void gpio_blanking_set(void);
void gpio_blanking_reset(void);
void gpio_polarity_set(void);
void gpio_polarity_reset(void);
void gpio_esp_set(void);
void gpio_esp_reset(void);
void gpio_set_ch_pd(void);
void gpio_reset_ch_pd(void);

void wdt_init(void);
void wdt_restart(void);

#endif // AVR_H_

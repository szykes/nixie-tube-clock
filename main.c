#include <avr/interrupt.h>

#include "gpio.h"
#include "wifi.h"
#include "led.h"
#include "clock.h"

int main()
{
  gpio_init();
  wifi_init();
  led_init();
  clock_init();

  sei();

  while(1) {
    clock_main();
  }
}

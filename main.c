#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "gpio.h"
#include "wifi.h"
#include "led.h"
#include "clock.h"

int main()
{
  sei();
  gpio_init();
  wifi_init();
  led_init();
  clock_init();
  wdt_enable(WDTO_250MS);

  while(1) {
    wifi_main();
    led_main();
    clock_main();
    wdt_reset();
  }
}

#include <avr/interrupt.h>
#include <avr/wdt.h>

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
  wdt_enable(WDTO_250MS);

  sei();

  while(1) {
    clock_main();
    wdt_reset();
  }
}

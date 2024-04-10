#include "avr.h"
#include "wifi.h"
#include "led.h"
#include "clock.h"

int main()
{
  mcu_sei();
  gpio_init();
  wifi_init();
  led_init();
  clock_init();
  wdt_init();

  while(1) {
    wifi_main();
    led_main();
    clock_main();
    wdt_restart();
  }
}

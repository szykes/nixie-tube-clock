#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "gpio.h"

ISR(TIMER1_OVF_vect) {
  if(PORTB)
    gpio_reset_led_red();
  else
    gpio_set_led_red();

  TCNT1 = 0xFF00;
}

void led_init(void) {
  TCCR1A = 0x00;
  /*  */
  TCCR1B = (1 << CS10);

  TCNT1 = 0xFF00;

  TIMSK1 = (1 << TOIE1);
}

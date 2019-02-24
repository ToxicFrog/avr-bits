// #define F_CPU 16500000L

#include <avr/io.h>
#include <util/delay.h>

// #include <avr/interrupt.h>
// #include <avr/wdt.h>

// #include "usbdrv.h"

#define LED_PIN (1<<PB1)

int main() {
  DDRB |= LED_PIN;
  PORTB |= LED_PIN;

  while (1) {
    _delay_ms(100);
    PORTB ^= LED_PIN;
  }

  return 0;
}

// #define F_CPU 16500000L

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

#define LED_PIN (1<<PB1)

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;

  switch(rq->bRequest) {
    case 1:
      PORTB |= LED_PIN;
      break;
    case 0:
      PORTB &= ~LED_PIN;
      break;
  }
  return 0; // should not get here
}

int main() {
  DDRB |= 1 | LED_PIN;
  PORTB |= 1 | LED_PIN;

  wdt_enable(WDTO_1S); // enable 1s watchdog timer

  usbInit();
  usbDeviceDisconnect(); // enforce re-enumeration
  for(uint8_t i = 0; i<250; i++) { // wait 500 ms
    wdt_reset(); // keep the watchdog happy
    _delay_ms(2);
  }
  usbDeviceConnect();

  PORTB &= ~LED_PIN;

  sei();
  while(1) {
    wdt_reset(); // keep the watchdog happy
    usbPoll();
  }

  return 0;
}

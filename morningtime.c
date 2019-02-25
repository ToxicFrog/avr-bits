// #define F_CPU 16500000L

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

#define RED_LED (1<<PB0)
#define PCB_LED (1<<PB1)
#define GRN_LED (1<<PB2)

void set_led(uint8_t led, uint8_t val) {
  if (val) {
    PORTB |= led;
  } else {
    PORTB &= ~led;
  }
}

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;

  switch(rq->bRequest) {
    case 1: // morning
      set_led(GRN_LED, 1);
      set_led(RED_LED, 0);
      break;
    case 0:
      set_led(RED_LED, 1);
      set_led(GRN_LED, 0);
      break;
  }
  return 0; // should not get here
}

int main() {
  DDRB |= RED_LED | GRN_LED | PCB_LED;
  set_led(PCB_LED, 1);

  wdt_enable(WDTO_1S); // enable 1s watchdog timer

  usbInit();
  usbDeviceDisconnect(); // enforce re-enumeration
  for(uint8_t i = 0; i<250; i++) { // wait 500 ms
    wdt_reset(); // keep the watchdog happy
    _delay_ms(2);
  }
  usbDeviceConnect();

  set_led(PCB_LED, 0);

  sei();
  while(1) {
    wdt_reset(); // keep the watchdog happy
    usbPoll();
  }

  return 0;
}

// #define F_CPU 16500000L

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

// Connections to RGBLED shield
#define RED_LED (1<<PB0)
#define PCB_LED (1<<PB1)
#define GRN_LED (1<<PB1)  // Shares a pin with the green onboard LED
#define BLU_LED (1<<PB2)

void set_led(uint8_t led, uint8_t val) {
  if (val) {
    PORTB |= led;
  } else {
    PORTB &= ~led;
  }
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  set_led(RED_LED, r);
  set_led(GRN_LED, g);
  set_led(BLU_LED, b);
}

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;

  switch(rq->bRequest) {
    case 1: // morning
      set_rgb(0, 1, 0);
      break;
    case 0:
      set_rgb(1, 0, 0);
      break;
  }
  return 0; // should not get here
}

int main() {
  DDRB |= RED_LED | GRN_LED | BLU_LED | PCB_LED;
  set_rgb(1,1,1);

  wdt_enable(WDTO_1S); // enable 1s watchdog timer

  usbInit();
  usbDeviceDisconnect(); // enforce re-enumeration
  for(uint8_t i = 0; i<250; i++) { // wait 500 ms
    wdt_reset(); // keep the watchdog happy
    _delay_ms(2);
  }
  usbDeviceConnect();

  set_rgb(0,0,0);

  sei();
  while(1) {
    wdt_reset();
    usbPoll();
  }

  return 0;
}

// #define F_CPU 16500000L

#include <ctype.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
    0x05, 0x01,  // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,  // USAGE (Keyboard)
    0xa1, 0x01,  // COLLECTION (Application)
    0x75, 0x01,  //   REPORT_SIZE (1)
    0x95, 0x08,  //   REPORT_COUNT (8)
    0x05, 0x07,  //   USAGE_PAGE (Keyboard)(Key Codes)
    0x19, 0xe0,  //   USAGE_MINIMUM (Keyboard LeftControl)(224)
    0x29, 0xe7,  //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
    0x15, 0x00,  //   LOGICAL_MINIMUM (0)
    0x25, 0x01,  //   LOGICAL_MAXIMUM (1)
    0x81, 0x02,  //   INPUT (Data,Var,Abs) ; Modifier byte
    0x95, 0x01,  //   REPORT_COUNT (1)
    0x75, 0x08,  //   REPORT_SIZE (8)
    0x81, 0x03,  //   INPUT (Cnst,Var,Abs) ; Reserved byte
    0x95, 0x05,  //   REPORT_COUNT (5)
    0x75, 0x01,  //   REPORT_SIZE (1)
    0x05, 0x08,  //   USAGE_PAGE (LEDs)
    0x19, 0x01,  //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,  //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,  //   OUTPUT (Data,Var,Abs) ; LED report
    0x95, 0x01,  //   REPORT_COUNT (1)
    0x75, 0x03,  //   REPORT_SIZE (3)
    0x91, 0x03,  //   OUTPUT (Cnst,Var,Abs) ; LED report padding
    0x95, 0x06,  //   REPORT_COUNT (6)
    0x75, 0x08,  //   REPORT_SIZE (8)
    0x15, 0x00,  //   LOGICAL_MINIMUM (0)
    0x25, 0x65,  //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,  //   USAGE_PAGE (Keyboard)(Key Codes)
    0x19, 0x00,  //   USAGE_MINIMUM (Reserved (no event indicated))(0)
    0x29, 0x65,  //   USAGE_MAXIMUM (Keyboard Application)(101)
    0x81, 0x00,  //   INPUT (Data,Ary,Abs)
    0xc0         // END_COLLECTION
};

typedef struct {
        uint8_t modifier;
        uint8_t reserved;
        uint8_t keycode[6];
} keyboard_report_t;

static keyboard_report_t keyboard_report; // sent to PC
static uint8_t idleRate; // repeat rate for keyboards

usbMsgLen_t usbFunctionSetup(uint8_t * data) {
  usbRequest_t *rq = (void *)data;

  if((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS) {
    return 0;
  }

  switch(rq->bRequest) {
    case USBRQ_HID_GET_REPORT: // send "no keys pressed" if asked here
      usbMsgPtr = (void *)&keyboard_report;
      keyboard_report.modifier = 0;
      keyboard_report.keycode[0] = 0;
      return sizeof(keyboard_report);
    case USBRQ_HID_SET_REPORT: // if wLength == 1, should be LED state
      return (rq->wLength.word == 1) ? USB_NO_MSG : 0;
    case USBRQ_HID_GET_IDLE: // send idle rate to PC as required by spec
      usbMsgPtr = &idleRate;
      return 1;
    case USBRQ_HID_SET_IDLE: // save idle rate as required by spec
      idleRate = rq->wValue.bytes[1];
      return 0;
  }

  return 0; // by default don't return any data
}

#define PCB_LED (1<<PB1)

void set_led(uint8_t led, uint8_t val) {
  if (val) {
    PORTB |= led;
  } else {
    PORTB &= ~led;
  }
}

#define NUM_LOCK (1<<0)
#define CAPS_LOCK (1<<1)
#define SCROLL_LOCK (1<<2)

volatile static uchar LED_state = 0xff;

usbMsgLen_t usbFunctionWrite(uint8_t * data, uchar len) {
  if (data[0] == LED_state)
    return 1;
  else
    LED_state = data[0];

  // LED state changed
  // set_led(PCB_LED, LED_state & NUM_LOCK);
  return 1; // Data read, not expecting more
}

enum ModifierKeys {
  L_CTRL  = 1<<0,
  L_SHIFT = 1<<1,
  L_ALT   = 1<<2,
  L_GUI   = 1<<3,
  R_CTRL  = 1<<4,
  R_SHIFT = 1<<5,
  R_ALT   = 1<<6,
  R_GUI   = 1<<7,
};

#define KEY_ERROR_UNDEFINED 0x03

#define KEY_ENTER 0x28 // Keyboard Return (ENTER)
#define KEY_ESC 0x29 // Keyboard ESCAPE
#define KEY_BACKSPACE 0x2a // Keyboard DELETE (Backspace)
#define KEY_TAB 0x2b // Keyboard Tab
#define KEY_SPACE 0x2c // Keyboard Spacebar
#define KEY_MINUS 0x2d // Keyboard - and _
#define KEY_EQUAL 0x2e // Keyboard = and +
#define KEY_LEFTBRACE 0x2f // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30 // Keyboard ] and }
#define KEY_BACKSLASH 0x31 // Keyboard \ and |
#define KEY_HASHTILDE 0x32 // Keyboard Non-US # and ~
#define KEY_SEMICOLON 0x33 // Keyboard ; and :
#define KEY_APOSTROPHE 0x34 // Keyboard ' and "
#define KEY_GRAVE 0x35 // Keyboard ` and ~
#define KEY_COMMA 0x36 // Keyboard , and <
#define KEY_DOT 0x37 // Keyboard . and >
#define KEY_SLASH 0x38 // Keyboard / and ?
#define KEY_CAPSLOCK 0x39 // Keyboard Caps Lock

// Given a letter, return the scancode and set modifiers accordingly.
// Returns 0 if it can't find the scancode.
uint8_t to_scancode(unsigned char letter, uint8_t * modifiers) {
  *modifiers = 0;
  if (isupper(letter)) {
    *modifiers = L_SHIFT;
    return 0x04 + (letter - 'A');
  } else if (letter >= 'a' && letter <= 'z') {
    return 0x04 + (letter - 'a');
  } else if (letter >= '1' && letter <= '9') {
    // 0 is handled in the switch below, because ASCII orders the digits 0-9
    // while USB orders them 1-0.
    return 0x1E + (letter - '1');
  } else switch(letter) {
    case '0': return 0x27;
    case ' ': return 0x2C;
    case '/': return 0x38;
    case '-': return 0x2D; // lowercase of _
    case '.': return 0x37;
    case '(': *modifiers = L_SHIFT; return 0x26;
    case ')': *modifiers = L_SHIFT; return 0x27;
    case ';': return 0x33;
    case '&': *modifiers = L_SHIFT; return 0x24;
    case '\n': return 0x28;
    case '\x1B': return 0x29; // ESC
    default: return 0;
  }
}

void send_report() {
  // Wait until ready
  while (!usbInterruptIsReady()) {
    wdt_reset();
    usbPoll();
    _delay_ms(1);
  }
  usbSetInterrupt((void*)&keyboard_report, sizeof(keyboard_report));
  while (!usbInterruptIsReady()) {
    wdt_reset();
    usbPoll();
    _delay_ms(1);
  }
}

void blink(size_t n) {
  for (; n; --n) {
    set_led(PCB_LED, 1);
    wdt_reset();
    usbPoll();
    _delay_ms(1);
    set_led(PCB_LED, 0);
    wdt_reset();
    usbPoll();
    _delay_ms(1);
  }
}

// Send a keydown event followed immediately by a keyup event.
void send_keyevent(uint8_t scancode, uint8_t modifiers) {
  keyboard_report.modifier = modifiers;
  keyboard_report.keycode[0] = scancode;
  set_led(PCB_LED, 1);
  send_report();

  memset(&keyboard_report, 0, sizeof(keyboard_report));
  set_led(PCB_LED, 0);
  send_report();
}

void send_letter(unsigned char letter) {
  uint8_t modifiers;
  uint8_t scancode = to_scancode(letter, &modifiers);
  if (!scancode) return;
  send_keyevent(scancode, modifiers);
}

void send_string(const char * str) {
  for (size_t i = 0; i < strlen(str); ++i) {
    send_letter(str[i]);
  }
}

void wait(size_t ms, uint8_t poll) {
  for (size_t i = 0; i < ms; i += 5) {
    wdt_reset();
    if (poll) usbPoll();
    _delay_ms(5);
  }
}

int main() {
  DDRB |= PCB_LED;

  // Turn on LED while initializing
  set_led(PCB_LED, 1);

  wdt_enable(WDTO_1S); // enable 1s watchdog timer

  usbInit();
  usbDeviceDisconnect(); // enforce re-enumeration
  wait(500, 0);
  usbDeviceConnect();

  sei();

  // Wait for device to settle, if we send keystrokes too soon they get lost
  wait(2000, 1);

  // Test pattern covers every main key on the keyboard.
  // Final column should be aligned if they all worked.
  send_string("~!@#$%^&*()_+  .\n");
  send_string("`1234567890-=  .\n");
  send_string("QWERTYUIOP{}|  .\n");
  send_string("qwertyuiop[]\\  .\n");
  send_string("ASDFGHJKL:\"    .\n");
  send_string("asdfghjkl;'    .\n");
  send_string("ZXCVBNM<>?     .\n");
  send_string("zxcvbnm,./     .\n");

  while(1) {
    wdt_reset();
    usbPoll();
  }

  return 0;
}

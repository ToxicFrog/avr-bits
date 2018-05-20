// Common header file that defines various constants and utility functions
// differently depending on what system we're building for.
// n.b. LINUX is defined if we're building for linux.
// AVR is defined if we're building for any AVR platform.
// ARDUINO is defined if we're building in the Arduino IDE.
// AVR_NO_ARDUINO is defined if we're building for AVR, but not in the Arduino IDE.

#pragma once

#include "config.h"

#ifdef LINUX
  #include <string.h>

  #define STACKSIZE LINUX_STACKSIZE
  #define PROGMEM
  #define PSTR(s) s
  #define pgm_read_byte(x) *(x)
  #define strcpy_P strcpy
  #define strcmp_P strcmp
  #define memcpy_P memcpy
  #define printf_P printf
  #define puts_P puts
  #define fputs_P fputs
#endif

#ifdef AVR
  #include <avr/pgmspace.h>

  #define STACKSIZE AVR_STACKSIZE

  #ifndef ARDUINO
    #define AVR_NO_ARDUINO
  #endif
#endif

#define STACKBYTES (STACKSIZE * sizeof(Cell))

#ifndef true
  #define false 0
  #define true (~false)
#endif

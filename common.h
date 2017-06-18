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

#ifdef ARDUINO
  #include <avr/pgmspace.h>

  #define STACKSIZE AVR_STACKSIZE
#endif

#define STACKBYTES (STACKSIZE * sizeof(Cell))

#ifndef true
  #define false 0
  #define true (~false)
#endif

#pragma once

#define false 0
#define true (~false)

#ifdef DEBUG
  #define debug(header, msg) print(header); println(msg)
  #define debugi(header, i) print(header); printint(i); println("")
#else
  #define debug(header, msg)
  #define debugi(header, i)
#endif

#if defined(LINUX) && defined(ARDUINO)
  #error "Can't #define both LINUX and ARDUINO at the same time"
#elif defined(LINUX)
  #define INPUT_BUFSIZE 1024
  #define STACKSIZE 1024
  #define PROGMEM
  #define strcpy_P(dst, src) abort()
  #define strcmp_P(s1, s2) abort(),1
  #define memcpy_P(dst, src, len) abort()
#elif defined(ARDUINO)
  #define INPUT_BUFSIZE 80
  #define STACKSIZE 32
  #include <avr/pgmspace.h>
#else
  #error "You must #define either LINUX or ARDUINO"
#endif

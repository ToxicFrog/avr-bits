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

// Maximum length of a word name. Used for some intermediate buffers.
// TODO: since the words are stored on the stack, we can use STACKSIZE for this.
#define NAMELEN 32

// Enable runtime safety checks for things like stack over/underflow and malloc
// failures. Small code size/runtime performance impact.
#define SAFETY_CHECKS

#if defined(LINUX) && defined(ARDUINO)
  #error "Can't #define both LINUX and ARDUINO at the same time"
#elif defined(LINUX)
  #include <string.h>

  #define INPUT_BUFSIZE 1024
  #define STACKSIZE 1024
  #define PROGMEM
  #define pgm_read_byte(x) *(x)
  #define strcpy_P(dst, src) strcpy(dst, src)
  #define strcmp_P(s1, s2) strcmp(s1, s2)
  #define memcpy_P(dst, src, len) memcpy(dst, src, len)
#elif defined(ARDUINO)
  #define INPUT_BUFSIZE 80
  #define STACKSIZE 32
  #include <avr/pgmspace.h>

  // Whether to enable the serial REPL for the Arduino build.
  // Turning this off disables the ability to run code by typing it into the
  // serial port, but also saves about 350b of RAM.
  #define SERIAL_REPL
#else
  #error "You must #define either LINUX or ARDUINO"
#endif

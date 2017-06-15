#pragma once

#include <setjmp.h>

#include "tty.h"

extern jmp_buf catchpoint;
#define error(msg) {\
  print(__FILE__); print(":"); printint(__LINE__); print(": error: "); println(msg) \
  longjmp(catchpoint, 1); \
}

#ifdef SAFETY_CHECKS
  #define CHECK(condition, msg) if (!(condition)) error(msg);
  #define CHECK_MALLOC(var, size, msg) var = malloc(size); CHECK(var != NULL, msg)
#else
  #define CHECK(condition, msg)
  #define CHECK_MALLOC(var, size, msg) var = malloc(size)
#endif

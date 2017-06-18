#pragma once

#include <setjmp.h>

#include "tty.h"

extern jmp_buf catchpoint;
#define error(fmt, ...) {\
  printf_P(PSTR(__FILE__ ":%u: " fmt "\n"), __LINE__, ##__VA_ARGS__); \
  longjmp(catchpoint, 1); \
}

#ifdef SAFETY_CHECKS
  #define CHECK(condition, ...) if (!(condition)) error(__VA_ARGS__);
  #define CHECK_MALLOC(var, size, what) var = malloc(size); \
            CHECK(var != NULL, "Can't allocate %lu bytes for " what ".", size)
#else
  #define CHECK(condition, ...)
  #define CHECK_MALLOC(var, size, what) var = malloc(size)
#endif

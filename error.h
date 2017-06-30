#pragma once

#include <setjmp.h>

#include "tty.h"

extern size_t  jmp_buf_index;
extern jmp_buf jmp_bufs[NROF_JMP_BUFS];

#define catch_error() setjmp(jmp_bufs[jmp_buf_index++])
#define uncatch_error() { jmp_buf_index--; }

#define qerror() longjmp(jmp_bufs[--jmp_buf_index], 1)

#define error(fmt, ...) {\
  printf_P(PSTR(__FILE__ ":%u: " fmt "\n"), __LINE__, ##__VA_ARGS__); \
  qerror(); \
}

#ifdef SAFETY_CHECKS
  #define CHECK(condition, ...) if (!(condition)) error(__VA_ARGS__);
  #define CHECK_MALLOC(var, size, what) var = malloc(size); \
            CHECK(var != NULL, "Can't allocate %lu bytes for " what ".", size)
#else
  #define CHECK(condition, ...)
  #define CHECK_MALLOC(var, size, what) var = malloc(size)
#endif

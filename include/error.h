#pragma once

#include <setjmp.h>

#include "tty.h"

extern size_t jmp_buf_index;
extern jmp_buf jmp_bufs[NROF_JMP_BUFS];

// Push a longjmp target onto the error handling stack and setjmp() it. Returns
// the return value of setjmp(), so the expected use is something like:
// if (catch_error()) { ... error recovery code ... }
// ... rest of code ...
// uncatch_error()
// All the usual caveats of setjmp() apply; see also uncatch_error() below for
// instructions on correctly unwinding the error handling stack.
#define catch_error() (++jmp_buf_index,setjmp(jmp_bufs[jmp_buf_index]))

// Pop the top of the error handling stack. Every call to catch_error MUST have
// a corresponding call to uncatch_error, ideally before returning from the
// stack frame in which catch_error() was called. Note that calling qerror() to
// jump back to the call to catch_error() DOES NOT implicitly pop the top of the
// error handling stack!
void uncatch_error();

// "Quiet error." Immediately longjmp() to the target on top of the error handling
// stack. DOES NOT implicitly pop that target.
void qerror();

// Output the given error message (same API as printf), prefixed with file and
// line information, then calls qerror().
#define error(fmt, ...) {\
  printf_P(PSTR(__FILE__ ":%u: " fmt "\n"), __LINE__, ##__VA_ARGS__); \
  qerror(); \
}

// Safety-checking functions that can be disabled in config.h.
// CHECK(condition, ...) is a no-op if condition is true, and if false,
// is equivalent to error(...).
// CHECK_MALLOC(var, size, desc) attempts to allocate `size` bytes and store
// the pointer in `var`. If allocation fails, calls error() with an error
// message containing both `size` and `desc`.
#ifdef SAFETY_CHECKS
  #define CHECK(condition, ...) if (!(condition)) error(__VA_ARGS__);
  #define CHECK_MALLOC(var, size, what) var = malloc(size); \
            CHECK(var != NULL, "Can't allocate %lu bytes for " what ".", size)
#else
  #define CHECK(condition, ...)
  #define CHECK_MALLOC(var, size, what) var = malloc(size)
#endif

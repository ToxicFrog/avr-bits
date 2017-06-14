#include "config.h"

#include "error.h"
#include "tty.h"

jmp_buf catchpoint;

// void error(const char* msg) {
//   print("Error: "); println(msg);
//   longjmp(catchpoint, 1);
// }

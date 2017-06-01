#ifdef HOST_NOTFORTH

#include <stdio.h>

#include "tty.h"

char next_char() {
  return getchar();
}

void error(const char * msg) {
  printf("Error: %s\n", msg);
}

#endif  // HOST_NOTFORTH

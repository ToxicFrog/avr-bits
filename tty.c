#ifdef HOST_NOTFORTH

#include <stdio.h>

#include "tty.h"

char next_char() {
  return getchar();
}

void error(const char * msg) {
  printf("Error: %s\n", msg);
}

void println(const char * msg) {
  printf("%s\n", msg);
}

void printint(const int n) {
  printf("%d\n", n);
}

#endif  // HOST_NOTFORTH

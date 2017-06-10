#include "tty.h"

#ifdef LINUX

#include <stdio.h>

char tty_peek() {
  int next = getc(stdin);
  if (next == EOF) {
    return '\0';
  }
  ungetc(next, stdin);
  return next;
}

char tty_next() {
  int next = getc(stdin);
  if (next == EOF) {
    return '\0';
  }
  printf("%c", next);
  return next;
}

void print(const char * msg) {
  printf("%s", msg);
}

void printint(const int n) {
  printf("%d", n);
}

#endif  // LINUX

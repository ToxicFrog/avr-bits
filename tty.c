#include "tty.h"

#ifdef LINUX

#include <stdio.h>

#include "error.h"

char tty_peek() {
  if (feof(stdin)) return '\0';
  int next = getc(stdin);
  ungetc(next, stdin);
  return next;
}

char tty_next() {
  int next = getc(stdin);
  CHECK(next != EOF, "Attempt to read past end of input.");
  return next;
}

void print(const char * msg) {
  printf("%s", msg);
}

void printint(const int n) {
  printf("%d", n);
}

#endif  // LINUX

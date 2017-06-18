#include "tty.h"

#include <stdio.h>

#ifdef LINUX

void tty_init() {}

int tty_fputc(char ch, FILE* tty) {
  return fputc(ch, tty);
}

int tty_fgetc(FILE* tty) {
  return fgetc(tty);
}

#else

// TODO definitions of tty_get/put here for AVR

#endif

int tty_peek() {
  int next = fgetc(stdin);
  if (next != EOF) ungetc(next, stdin);
  return next;
}

int tty_next() {
  return fgetc(stdin);
}

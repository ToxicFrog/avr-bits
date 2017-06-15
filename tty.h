// Functions to talk to the terminal. These are defined in tty.ino (AVR) or
// tty.c (linux).

#pragma once
#include <stdlib.h>

char tty_peek();
char tty_next();
int tty_eof();

void print(const char *);
#define println(str) { print(str); print("\r\n"); }
void printint(const int);

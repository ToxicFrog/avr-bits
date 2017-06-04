// Functions to talk to the terminal. These are defined in tty.ino (AVR) or
// tty.c (linux).

#pragma once
#include <stdlib.h>

char next_char();
void print(const char *);
#define println(str) { print(str); print("\r\n"); }
void printint(const int);
void readline(char *, size_t);

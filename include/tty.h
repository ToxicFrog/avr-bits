// Functions to talk to the terminal. These are defined in tty.ino (AVR) or
// tty.c (linux).

#pragma once

#include <stdlib.h>
#include <stdio.h>

void tty_init();

int tty_fputc(char ch, FILE* tty);
int tty_fgetc(FILE* tty);

int tty_peek();
int tty_next();

#ifndef SERIAL_REPL

#define printf(...)

#endif

// Functions to talk to the terminal. These are defined in tty.ino (AVR) or
// tty.c (linux).

#pragma once

char next_char();
void error(const char *);
void println(const char *);
void printint(const int);

#ifdef HOST_NOTFORTH
#define F(s) s
#endif

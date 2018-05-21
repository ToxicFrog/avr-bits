#include "common.h"

#include "tty.h"

#include <stdio.h>

int tty_peek() {
  int next = fgetc(stdin);
  if (next != EOF) ungetc(next, stdin);
  return next;
}

int tty_next() {
  return fgetc(stdin);
}


#ifndef SERIAL_REPL

void tty_init() {}
int tty_fputc(char ch, FILE* tty) { return EOF; }
int tty_fgetc(FILE* tty) { return EOF; }

#elif defined(LINUX)

void tty_init() {}

int tty_fputc(char ch, FILE* tty) {
  return fputc(ch, tty);
}

int tty_fgetc(FILE* tty) {
  return fgetc(tty);
}

#else

#include <avr/io.h>

#ifndef F_CPU
  #warning F_CPU not defined, assuming 16MHz
  #define F_CPU 16000000UL
#endif

#define BAUD 9600

#include <util/setbaud.h>

/* old serial setup code
  UCSR0A=0x00;
  UCSR0B=0x18;
  UCSR0C=0x86;
  UBRR0=0x0019;
*/

void tty_init() {
  // UBRR*_VALUE and USE_2X provided by setbaud.h
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */

  fdevopen(tty_fputc, tty_fgetc);
}

int tty_fputc(char c, FILE* tty) {
  if (c == '\n') {
      tty_fputc('\r', tty);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return c;
}

int tty_fgetc(FILE* tty) {
  loop_until_bit_is_set(UCSR0A, RXC0);
  // Echo back to the typist
  int ch = UDR0;
  if (ch == '\r')
    // Compensate for weird serial clients like screen that send \r
    ch = '\n';
  tty_fputc(ch, tty);
  return ch;
}

#endif

#include "common.h"

#ifdef SERIAL_REPL

extern "C" void tty_init() {
  Serial.begin(9600);

  while(!Serial) {
    delay(100);
  }

  fdevopen(tty_fputc, tty_fgetc);
}

extern "C" int tty_fputc(char ch, FILE* tty) {
  return Serial.write(ch);
}

extern "C" int tty_fgetc(FILE* tty) {
  while (!Serial.available()) {
    delay(10);
  }
  int ch = Serial.read();
  tty_fputc(ch, tty);
  return ch;
}

#else

#define printf(...)

extern "C" void tty_init() {}
extern "C" char tty_fputc(char ch, FILE* tty) { return EOF; }
extern "C" char tty_fgetc(FILE* tty) { return EOF; }

#endif

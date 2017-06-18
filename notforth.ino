#include "common.h"

extern "C" {
#include "dictionary.h"
#include "repl.h"
#include "corewords.h"
}

void setup() {
  load_core_words();

#ifdef SERIAL_REPL
  tty_init();
#endif SERIAL_REPL
}

void loop() {
#ifdef SERIAL_REPL
  repl();
#endif
}

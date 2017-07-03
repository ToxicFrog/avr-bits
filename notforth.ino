#include "common.h"

extern "C" {
  #include "dictionary.h"
  #include "repl.h"
  #include "corewords.h"
  #include "tty.h"
}

void setup() {
  load_core_words();

#ifdef SERIAL_REPL
  tty_init();
  printf_P(PSTR("Startup complete.\n"));
#endif
}

void loop() {
#ifdef SERIAL_REPL
  repl();
#endif
}

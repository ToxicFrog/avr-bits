#include "common.h"

extern "C" {
  #include "dictionary.h"
  #include "execute.h"
  #include "corewords.h"
  #include "tty.h"
}

void setup() {
  load_core_words();

#ifdef SERIAL_REPL
  tty_init();
  printf_P(PSTR("Ready.\n"));
#endif
}

void loop() {
#ifdef SERIAL_REPL
  execute_word(find_word("main"));
#endif
}

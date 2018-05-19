// Notforth main file for non-Arduino AVR builds.
// This doesn't mean it won't run on the Arduino, just that it's being built
// outside the Arduino IDE and thus needs to supply its own main() and
// startup code.
// See notforth.ino for Arduino-specific code and main.c for hosted.

#include "common.h"

#ifdef AVR_NO_ARDUINO

#include "dictionary.h"
#include "corewords.h"
#include "tty.h"
#include "execute.h"

void main() {
  load_core_words();
#ifdef SERIAL_REPL
  tty_init();
  printf_P(PSTR("Ready.\n"));
#endif
  DDRB |= 1<<5;
  PORTB |= 1<<5;

  execute_word(find_word("main"));
}

#endif  // AVR_NO_ARDUINO

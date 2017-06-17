#include "common.h"

extern "C" {
#include "dictionary.h"
#include "repl.h"
#include "corewords.h"
}

void setup() {
  load_core_words();

#ifdef SERIAL_REPL
  Serial.begin(9600);

  while(!Serial) {
    delay(100);
  }

  Serial.println(F("Initialization complete."));
  Serial.print(F("Pointer size: ")); Serial.println(sizeof(void*), 16);
  Serial.print(F("Word size: ")); Serial.println(sizeof(Word));
#endif SERIAL_REPL
}

void loop() {
#ifdef SERIAL_REPL
  repl();
#endif
}

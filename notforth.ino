extern "C" {
#include "dictionary.h"
#include "repl.h"
#include "corewords.h"
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  load_core_words();

  while(!Serial) {
    delay(100);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("Initialization complete."));
  Serial.print(F("Pointer size: ")); Serial.println(sizeof(void*), 16);
  Serial.print(F("Word size: ")); Serial.println(sizeof(Word));
}

void loop() {
  repl();
}

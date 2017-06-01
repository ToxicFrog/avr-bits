extern "C" {
#include "dictionary.h"
#include "repl.h"
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  while(!Serial) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Initialization complete.");
  Serial.print("Pointer size: "); Serial.println(sizeof(void*), 16);
  Serial.print("Word size: "); Serial.println(sizeof(Word));
  Serial.println("");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  while(1) {
    Serial.print("] ");
    process_line();
  }
}

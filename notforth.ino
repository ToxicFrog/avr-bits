#include "dictionary.h"

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Initialization complete.");
  Serial.print("Pointer size: "); Serial.println(sizeof(void*), 16);
  Serial.print("Word size: "); Serial.println(sizeof(Word));
  Serial.println("");
}

void loop() {
  // put your main code here, to run repeatedly:
  sleep(1000);
}

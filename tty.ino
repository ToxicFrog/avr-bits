extern "C" char tty_next() {
  while (!Serial.available()) {
    delay(10);
  }
  char ch = (char)Serial.read();
  // Terminal is in raw mode by default, and serial convention is for enter on
  // the terminal to send CR and for the device to reply CR LF when ready.
  // So, a bit of a nasty hack here:
  if (ch == '\r') {
    Serial.write("\r\n");
    ch = '\n';
  } else {
    Serial.write(ch);
  }

  return ch;
}

extern "C" char tty_peek() {
  while (!Serial.available()) {
    delay(10);
  }
  return Serial.peek();
}

extern "C" void print(const char * msg) {
  Serial.print(msg);
}

extern "C" void printint(const int n) {
  Serial.print(n);
}

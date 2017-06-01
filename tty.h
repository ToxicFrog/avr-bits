// Compatibility shim so that we can call things like getchar() and have them
// turn into calls to the serial port instead. Actual function definitions are
// in serial_stdio.ino

char next_char();
void error(const char *);

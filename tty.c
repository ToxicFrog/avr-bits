#include "tty.h"

#ifdef LINUX

#include <stdio.h>

char next_char() {
  return getchar();
}

void print(const char * msg) {
  printf("%s", msg);
}

void printint(const int n) {
  printf("%d", n);
}

#endif  // LINUX

// Read up to len bytes from input and store them in buf. Stops when buf is
// full or newline is entered. Null terminates buf (so buf should have size len+1).
// Does not store the final newline, but does store other whitespace.
void readline(char* buf, size_t len) {
  size_t n = 0;
  char comment = '\0';
  for (char ch = next_char();
       n < len && ch != '\n';
       ch = next_char()) {
    if (ch == '\b' || ch == '\x7F') {
      buf[--n] = '\0'; continue;
    }
    if (ch == '#' || ch == '(') comment = ch;
    if (!comment) buf[n++] = ch;
    if (ch == ')' && comment == '(') comment = '\0';
  }
  buf[n] = '\0';
}

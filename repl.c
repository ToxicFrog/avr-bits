#include <ctype.h>  // isspace, etc
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

#include "repl.h"
#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

#define debuglog(...)

jmp_buf catch;

void die(const char * msg) {
  println(msg);
  longjmp(catch, 1);
}

// Run the word at the head of the buffer.
// On error, report the error and then longjmp.
// Return pointer to first character past word.
char * run_word(char * buf) {
  // Is it a comment?
  // TODO: drop comments at user input time
  if (buf[0] == '#') {
    return buf + strlen(buf);
  }
  if (buf[0] == '(') {
    char * end = strchr(buf, ')');
    if (!end) die("error: unmatched start-of-comment character");
    return end+1;
  }

  // Is it a numeric constant?
  if (isdigit(buf[0])
      || (isdigit(buf[1]) && (buf[0] == '-' || buf[0] == '+'))) {
    char * end;
    push(strtol(buf, &end, 0));
    return end;
  }

  // TODO: is it a string?

  // Otherwise it's a word.
  char * end = buf;
  while (!isspace(*end) && *end != '\0') ++end;
  // At this point it points to either the end of the buffer, which is already \0,
  // or to the whitespace separating this word from the next, which we can safely
  // discard
  if (*end) *(end++) = '\0';

  // Now buf points to the start of the word and is null terminated
  Word* word = find_word(buf);
  if (!word) {
    print("Word not found in dictionary: ");
    die(buf);
  }
  execute_word(word);
  return end;
}

#define INPUT_BUFSIZE 80

// Execute the code inside buf.
// MODIFIES THE CONTENTS OF BUF. Handle with care.
void run_buffer(char * buf) {
  if(setjmp(catch)) {
    // An error was thrown inside the execution loop.
    return;
  }
  // process each word in the buffer
  while (1) {
    // Skip leading whitespace
    while (isspace(*buf)) ++buf;
    if (*buf == '\0') return;
    buf = run_word(buf);
  }
}

void repl() {
  char buf[INPUT_BUFSIZE+1];
  while (1) {
    print("] ");
    readline(buf, INPUT_BUFSIZE);
    run_buffer(buf);
  }
}

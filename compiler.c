#include "config.h"

#include <stdlib.h>

#include "compiler.h"
#include "dictionary.h"
#include "execute.h"

// Stack of Word definitions that we're compiling. When we finish a word, it
// gets popped from this stack and added to DICTIONARY, albeit without a name.
// It's up to the user to call defn to give it one, if they want.
Word* compiling = NULL;

// Create a new, blank Word definition and put it on the compilation stack.
void word_beginfn() {
  Word* word = malloc(sizeof(Word));
  word->next = compiling;
  word->execute = NULL;
  word->name = NULL;
  word->flags = 0;
  compiling = word;
}

// Finalize a word definition: pop all bytecode on the data stack back to the
// last {, copy it into the Word on top of the compilation stack, then link
// the Word into the dictionary without a name and push it onto the data stack.
// TODO: if running on the host and in c/file mode, also emit an appropriate
// C function definition for it.
void word_endfn() {
  if (!compiling) return; //die("} with no corresponding {");

  // Pop it from the compilation stack.
  Word* word = compiling;
  compiling = word->next;

  // Copy the wordlist off the stack.
  // The top word->flags stack cells contain instructions.
  // This relies on Cell and Word* having the same size, but since Cell is intptr_t that should be the case?
  size_t len = (size_t)(word->flags);
  Word* body = calloc(len + 1, sizeof(Word*));
  memcpy(body, &STACK[STACKP-len], len * sizeof(Cell));
  word->execute = (WordImpl)body;

  // Drop the wordlist from the stack.
  STACKP -= len;

  // Set flags.
  word->flags = IS_WORDLIST;

  // Link the word into the dictionary.
  word->next = DICTIONARY;
  DICTIONARY = word;

  push((Cell)word);
}

#ifdef LINUX

// Code for compiling C words goes here.

#ifdef LINUX

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

const char * mangle(const char * name) {
  static char mangled_name[NAMELEN*3+1];
  // Determine the mangled name
  char* mch = mangled_name;
  for (const char* ch = name; *ch; ++ch) {
    if (isalnum(*ch)) {
      *(mch++) = *ch;
    } else {
      *(mch++) = 'X';
      mch += sprintf(mch, "%X", *ch);
    }
  }
  *mch = '\0';
  return mangled_name;
}

// Each one has two parts, the impl and the header
// the impl has the form:
// #includes
// list of function definitions
// dictionary fragment definition
// the header has the form:
// extern declaration of dictionary fragment
// redefinition of LAST_DICT and LAST_DICT_LEN macros
FILE * cimpl = NULL;
FILE * cdict = NULL;
char cname[NAMELEN*3+1];
size_t nrof_cdefs = 0;

void word_cfile() {
  // write #include impl to the start of the dict?
  char* name = (char*)pop();
  if (name) {
    sprintf(cname, "%s.impl", name);
    cimpl = fopen(cname, "w");
    sprintf(cname, "%s.dict", name);
    cdict = fopen(cname, "w");

    strcpy(cname, mangle(name));

    fprintf(cdict, "// Words from %s\n\n", name);
    free(name);
  } else {
    // Finalize everything.

    Word* cdef = DICTIONARY;
    while (nrof_cdefs) {
      nrof_cdefs--;
      const char* mangled_name = mangle(cdef->name);
      fprintf(cdict, "  { (Word*)1, word_%s_impl, word_%s_name, SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH | %d },\n",
        mangled_name, mangled_name, cdef->flags & ~IS_WORDLIST);
      do { cdef = next_word(cdef); } while(!cdef->name);
    }
    fclose(cdict);
    fclose(cimpl);
  }
}

// Similar to defn, but assumes the top of the stack is a function full of string
// literals. Write those out to the C files selected with c/file and save the
// function itself as a no-op. The strings are freed.
void word_cdefn() {
  assert(cimpl);
  char* body = (char*)pop();
  const char* name = (char*)pop();
  const char* mangled_name = mangle(name);

  // Write the implementation
  fprintf(cimpl, "\nconst PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, name);
  fprintf(cimpl, "void word_%s_impl() {\n%s\n}\n", mangled_name, body);
  free(body);

  register_word(name, NULL)->flags |= IS_WORDLIST;
  ++nrof_cdefs;
}

#endif



#endif

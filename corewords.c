// Core words for notforth.

#include "config.h"

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

#define ENABLE_BUILTINS
#include "builtins/all.c"
#undef ENABLE_BUILTINS

//// Control words ////

void printstr_P(const char * str) {
  char ch[2] = { 0, 0 };
  while ((ch[0] = pgm_read_byte(str++))) {
    print(ch);
  }
}

void printname(const Word* word) {
  if (!word->name) {
    print("(anonymous)");
    return;
  }
  if (word->flags & NAME_IN_FLASH) {
    printstr_P(word->name);
  } else {
    print(word->name);
  }
}

void word_words() {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    printint((intptr_t)word); print(":"); printint((intptr_t)word->execute);
    print(" ");
    printname(word);
    print(" ["); printint(word->flags); println("]");
  }
}

void impl_to_wordname(WordImpl impl) {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    if (word->execute == impl) {
      print(" :"); printname(word);
      return;
    }
  }
}

void word_list() {
  Word* word = (Word*)pop();
  print("# word "); printname(word); println("");
  if (word->flags & IS_CONSTANT) {
    print("constant "); printint((intptr_t)word->execute); println("");
  } else if (word->flags & IS_WORDLIST) {
    WordImpl* op = (WordImpl*)word->execute;
    while (*op != NULL) {
      if (*op == (WordImpl)0x0001) {
        print("pushliteral "); printint((intptr_t)*(++op)); println("");
      } else if (*op == (WordImpl)0x0002) {
        print("call word@"); printint((intptr_t)*(++op)); print(" "); printname((Word*)*op); println("");
      } else {
        print("call C@"); printint((intptr_t)*op); impl_to_wordname(*op); println("");
      }
      ++op;
    }
  } else {
    print("call C@"); printint((intptr_t)word->execute); println("");
  }
  println("# end");
}

#ifdef LINUX

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

void word_bye() {
  exit(0);
}

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
  Word* word = (Word*)pop();
  const char* name = (char*)pop();
  const char* mangled_name = mangle(name);

  // Write the implementation
  fprintf(cimpl, "\nconst PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, name);
  fprintf(cimpl, "void word_%s_impl() {\n", mangled_name);
  for (char ** line = (char**)word->execute; *line; line += 2) {
    fprintf(cimpl, "  %s\n", line[1]); // line[0] is going to be OP_PUSHLITERAL
  }
  fprintf(cimpl, "}\n");

  word->name = name;
  free(word->execute);
  word->execute = NULL;
  word->flags = IS_WORDLIST;
  ++nrof_cdefs;
}

#endif

// the { that begins a function definition
// switches the core into compile mode, if it's not already there, and pushes
// a new, empty wordlist function onto the definition stack (can this be the same as the data stack?)
void word_beginfn() {
  defn_begin();
}

// the } that ends a function definition
// switches the core out of compile mode, if this is the outermost function we're defining
// allocates a Word for the function and pushes it onto the stack
// The Word is added to the dictionary so it can be freed later, but has no name
// To give it a name, use defn once it's on the stack.
void word_endfn() {
  push((Cell)defn_end());
}

const PROGMEM Word CORE_WORDS[] = {
  { (Word*)1, word_beginfn, "{", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE },
  { (Word*)1, word_endfn, "}", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE},

  #define ENABLE_BUILTINS
  #include "builtins/all.h"
  #undef ENABLE_BUILTINS

#ifdef LINUX
  { (Word*)1, word_cfile, "c/file", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_cdefn, "c/defn", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_bye, "bye", NEXT_IN_FLASH | SELF_IN_FLASH },
#endif

  { NULL, word_list, "list", SELF_IN_FLASH },
};

void load_core_words() {
  DICTIONARY = (Word*)CORE_WORDS;  // cast to remove the const
  // We place `words` outside flash both because we need at least one dictionary
  // entry in RAM to contain the first NEXT_IN_FLASH, and because it's useful to
  // have available ~always for troubleshooting issues with flash dictionary traversal.
  register_word("words", word_words)->flags |= NEXT_IN_FLASH;
}

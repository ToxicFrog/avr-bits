// Library for compiling functions into C code.
// See c_compiler.h for API details.

#ifdef LINUX

#include "common.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "c_compiler.h"
#include "dictionary.h"
#include "execute.h"
#include "error.h"
#include "compiler.h"

const char * mangle(const char * name) {
  // The maximum possible length of a name -- assuming no shenanigans are happening
  // in C -- is the number of characters you can fit on the stack, x3 if they all
  // have to be X-escaped due to not being alphanumeric.
  static char mangled_name[STACKBYTES * 3];
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
char cname[STACKBYTES];

void word_cfile() {
  char* name = (char*)pop();
  if (name) {
    CHECK(!cimpl,
      "Attempt to open %s.{dict,impl} when there's already a c/file open.",
      name);
    sprintf(cname, "%s.impl", name);
    cimpl = fopen(cname, "w");
    sprintf(cname, "%s.dict", name);
    cdict = fopen(cname, "w");

    strcpy(cname, mangle(name));

    fprintf(cdict, "// Words from %s\n\n", name);
    free(name);
  } else {
    // Finalize everything.
    CHECK(cimpl, "Attempt to close a c/file without one open.");
    fclose(cdict); cdict = NULL;
    fclose(cimpl); cimpl = NULL;
  }
}

// Similar to defn, but assumes the top of the stack is a function full of string
// literals. Write those out to the C files selected with c/file and save the
// function itself as a no-op. The strings are freed.
void word_cdefn() {
  CHECK(cimpl, "Attempt to call c/impl before c/file.");
  char* body = (char*)pop();
  const char* name = (char*)pop();
  const char* mangled_name = mangle(name);

  // Write the implementation
  fprintf(cimpl, "\nconst PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, name);
  fprintf(cimpl, "void word_%s_impl() {\n%s\n}\n", mangled_name, body);
  free(body);

  // Write the dictionary entry.
  fprintf(cdict, "  { (Word*)1, word_%s_impl, word_%s_name, SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH },\n",
    mangled_name, mangled_name);

  // Store a no-op version in the dictionary. We can't call it without recompiling,
  // but this lets us compile references to it, so functions later in a file can
  // refer to functions defined earlier in it.
  register_word(name, NULL, IS_BYTECODE);
}

// Functions for compiling words written in notforth into C.

// How much space we allocate for a function definition.
#define C_IMPL_BUFSIZE (size_t)4096
#include <stdarg.h>

void c_append(const char * fmt, ...) {
  //printf("c_append: %s\n", fmt);
  if (!compiling || !cimpl) return;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char*)compiling->name + strlen(compiling->name), fmt, ap);
  va_end(ap);
  //printf("c_append: ok\n");
}

void c_beginfn(Word* word) {
  if (!compiling || !cimpl) return;
  CHECK_MALLOC(compiling->name, C_IMPL_BUFSIZE, "buffer for C source code");
  ((char*)compiling->name)[0] = '\0';
  c_append("\nvoid word_anon_%p() {\n", word);
}

void c_endfn(Word* word) {
  if (!compiling || !cimpl) return;
  c_append("}\n");
  fprintf(cimpl, "%s", (char*)word->name);
  free((char*)word->name);
  word->name = NULL;
}

void c_register_word(Word* word) {
  if (!cimpl) return;
  const char* mangled_name = mangle(word->name);
  fprintf(cimpl, "#define word_%s_impl word_anon_%p\n",
    mangled_name, word);
  fprintf(cimpl, "const PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, word->name);
  // We force the IS_BYTECODE flag off here because once this definition gets
  // loaded at compile time, it isn't bytecode -- but this is probably getting
  // called from `defn`, which means the function *in memory* is implemented as
  // bytecode and that flag will be set.
  fprintf(cdict,
    "  { (Word*)1, word_%s_impl, word_%s_name, "
    "(SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH | %d) & ~IS_BYTECODE },\n",
    mangled_name, mangled_name, word->flags);
}

void c_pushstring(const char* str) {
  c_append("  push((Cell)\"%s\");\n", str);
}

void c_pushnumber(Cell n) {
  c_append("  push(%d);\n", n);
}

// FIXME: this is wrong. It pushes the address of the word's implementation
// function, not the address of the word's dictionary entry.
void c_pushword(Word* word) {
  if (word->name) {
    c_append("  push((Cell)word_%s_impl);\n", mangle(word->name));
  } else {
    c_append("  push((Cell)word_anon_%p);\n", word);
  }
}

void c_callword(Word* word) {
  if (word->name) {
    c_append("  word_%s_impl();\n", mangle(word->name));
  } else {
    c_append("  word_anon_%p();\n", word);
  }
}


#endif  // LINUX

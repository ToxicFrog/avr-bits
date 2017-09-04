#pragma once

#include "dictionary.h"
#include "execute.h"

#define OP_EOF ((WordImpl)0)
#define OP_PUSHLITERAL ((WordImpl)-1)
#define OP_CALLWORD ((WordImpl)-2)

extern Word* compiling;

void word_beginfn(void);
void word_endfn(void);

#ifdef LINUX
void word_cfile(void);
void word_cdefn(void);
void c_register_word(Word*);
#else
#define c_register_word(x)
#endif

void compile_string(const char * str);
void compile_number(Cell num);
void compile_word(Word* word);
void compile_addressof(Word* word);
void compile_eof();
void compile_abort();

#pragma once

#include "dictionary.h"

#define OP_EOF ((WordImpl)0x0000)
#define OP_PUSHLITERAL ((WordImpl)0x0001)
#define OP_CALLWORD ((WordImpl)0x0002)

extern Word* compiling;

void word_beginfn(void);
void word_endfn(void);

#ifdef LINUX
void word_cfile(void);
void word_cdefn(void);
#endif

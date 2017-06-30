#include "common.h"

#include "error.h"
#include "tty.h"

size_t jmp_buf_index = 0;
jmp_buf jmp_bufs[NROF_JMP_BUFS];

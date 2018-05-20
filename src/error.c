#include "common.h"

#include "error.h"
#include "tty.h"

// The index points to the topmost jmp_buf that is safe to be used as a jump
// target; it's -1 if there are no valid targets.
size_t jmp_buf_index = -1;
jmp_buf jmp_bufs[NROF_JMP_BUFS];

void qerror() {
  longjmp(jmp_bufs[jmp_buf_index], 1);
}

void uncatch_error() {
  --jmp_buf_index;
}

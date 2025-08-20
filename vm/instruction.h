#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "vm.h"

Err inst_nop(VM* vm);
Err inst_hlt(VM* vm);
Err inst_mov(VM* vm);
Err inst_add(VM* vm);
Err inst_sub(VM* vm);

#endif // INSTRUCTION_H_


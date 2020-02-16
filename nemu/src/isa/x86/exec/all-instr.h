#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
// 2020年02月16日11:49:05 add 
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(jmp);
make_EHelper(call);
make_EHelper(ret);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(nop);
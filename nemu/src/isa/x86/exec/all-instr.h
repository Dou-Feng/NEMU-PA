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
// 2020年02月16日17:06:42
make_EHelper(setcc);
make_EHelper(jcc);
make_EHelper(cmp);
make_EHelper(test);
make_EHelper(and);
make_EHelper(or);
make_EHelper(add);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(leave);
//2020年02月16日21:01:18
make_EHelper(imul1);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(shl);
make_EHelper(sar);
make_EHelper(shr);
make_EHelper(not);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(mul);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(call_rm);
make_EHelper(jmp_rm);
// 2020年02月17日13:31:17
make_EHelper(neg);
make_EHelper(out);
make_EHelper(in);
make_EHelper(rol);

// 2020年02月21日21:02:08
make_EHelper(int);
make_EHelper(lidt);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(iret);

make_EHelper(mov_r2cr);
make_EHelper(mov_cr2r);
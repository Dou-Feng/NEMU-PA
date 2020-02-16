#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0, &id_dest->val, &id_src->val);

  // don't write the answer back 

  // update CF, OF
  rtl_li(&s1, 0);
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  
  // update ZF, SF
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(test);

}

make_EHelper(and) {
  rtl_and(&s0, &id_dest->val, &id_src->val);

  // write the answer back 
  operand_write(id_dest, &s0);

  // update CF, OF
  rtl_li(&s1, 0);
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  
  // update ZF, SF
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&s0, &id_dest->val, &id_src->val);

  // write the answer back 
  operand_write(id_dest, &s0);

  // update CF, OF
  rtl_li(&s1, 0);
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  
  // update ZF, SF
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&s0, &id_dest->val, &id_src->val);

  // write the answer back 
  operand_write(id_dest, &s0);

  // update CF, OF
  rtl_li(&s1, 0);
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  
  // update ZF, SF
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  // do sign extend
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
  
  // shift bits
  rtl_sar(&s0, &id_dest->val, &id_src->val);

  // write the result back
  operand_write(id_dest, &s0);

  // update ZF SF
  rtl_update_ZFSF(&s0, id_dest->width);


  print_asm_template2(sar);
}

make_EHelper(shl) {
  // need to optimise
  // set CF
  switch(id_dest->width) {
    case 1: rtl_li(&s0, 0x80); break;
    case 2: rtl_li(&s0, 0x8000); break;
    case 4: rtl_li(&s0, 0x80000000); break;
    default: panic("Not supported width");
  }
  // update CF
  rtl_li(&s1, (id_dest->val & (s0 >> (id_src->val - 1)))?1:0);
  rtl_set_CF(&s1);

  // shift bit
  rtl_shl(&s1, &id_dest->val, &id_src->val);

  // write the result back
  operand_write(id_dest, &s1);

  // update ZF SF
  rtl_update_ZFSF(&s1, id_dest->width);

  // update OF
  if (id_src->val == 1) {
    // the the sign bit of result(s1)
    rtl_li(&s0, (s1 & s0)?1:0);
    rtl_get_CF(&s1);
    rtl_xor(&s0, &s0, &s1);
    rtl_set_OF(&s0);
  }

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  // do unsigned extend, maybe not necessary
  // TODO();

  // shift bits
  rtl_shr(&s0, &id_dest->val, &id_src->val);

  // write the result back
  operand_write(id_dest, &s0);

  // update ZF SF
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;
  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);
  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&id_dest->val, &id_dest->val);
  
  // write the result back
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(not);
}

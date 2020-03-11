#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  // store data to IDTR.limit
  rtl_lm(&s0, &id_dest->addr, 2);
  IDTR(limit) = (unsigned short)s0 & 0xffff;
  rtl_addi(&id_dest->addr, &id_dest->addr, 2);
  rtl_lm(&s0, &id_dest->addr, 4);
  if (decinfo.isa.is_operand_size_16) {
    IDTR(base) = (0xffffff & s0);
  } else {
    IDTR(base) = s0;
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  Log("the rc no is %d, reg no is %d, val is 0x%x", id_dest->reg, id_src->reg, id_src->val);
  cpu.crs[id_dest->reg] = id_src->val;

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  
  rtl_li(&s0, cpu.crs[id_src->reg]);
  // Log("the rc no is %d, reg no is %d, val is 0x%x", id_dest->reg, id_src->reg, s0);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  raise_intr(id_dest->val, *pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  // pop ip
  rtl_pop(&s0);
  rtl_j(s0);
  // pop cs
  rtl_pop(&s0);
  CS = s0;
  // pop eflags
  rtl_pop(&s0);
  EFLAGS = s0;

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  switch (id_dest->width)
  {
    case 1:
      s0 = pio_read_b(id_src->val);
      break;
    case 2:
      s0 = pio_read_w(id_src->val);
      break;
    case 4:
      s0 = pio_read_l(id_src->val);
    default:
      break;
  }
  // write the answer back
  operand_write(id_dest, &s0);
  
  print_asm_template2(in);
}

make_EHelper(out) {
  // Output the src value to the port of id_dest->val point to
  // Log("the dest is 0x%x", id_dest->val);
  switch (id_src->width)
  {
    case 1:
      pio_write_b(id_dest->val, id_src->val);
      break;
    case 2:
      pio_write_w(id_dest->val, id_src->val);
      break;
    case 4:
      pio_write_l(id_dest->val, id_src->val);
    default:
      break;
  }
  print_asm_template2(out);
}

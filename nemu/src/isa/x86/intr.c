#include "rtl/rtl.h"

#define IDT_SIZE 8

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // push eflags
  rtl_li(&s0, EFLAGS);
  rtl_push(&s0);
  // set IF, TF = 0
  EFLAG(IF) = 0;
  EFLAG(TF) = 0;
  // push cs
  rtl_li(&s0, CS);
  rtl_push(&s0);
  // push ip
  rtl_li(&s0, ret_addr);
  rtl_push(&s0);
  // dont care CS, just modify the IP
  // get eip from IDT
  s0 = vaddr_read(IDTR(base) + NO * IDT_SIZE, 2);
  s1 = vaddr_read(IDTR(base) + (NO+1) * IDT_SIZE - 2, 2);
  s1 = (s1 << 16) + s0; // high bits ## low bits
  rtl_j(s1);
}

bool isa_query_intr(void) {
  return false;
}

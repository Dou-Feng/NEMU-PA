#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool ret = false;
  for (int i = 0; i < 8; i++) {
    if (ref_r->gpr[i]._32 != reg_l(i)) {
      Log("ref_r->gpr[%d] != cpu.gpr[%d]. ref_r->gpr[%d] = 0x%x", i, i, i, ref_r->gpr[i]._32);
      ret = true;
    }
  }
  if (ref_r->pc != cpu.pc) {
    Log("ref_r->pc != cpu.pc");
    return false;
  }
  return !ret;
}

extern void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
extern void (*ref_difftest_setregs)(const void *c);
#define ISA_DIFF_END1 0x7c00
#define ISA_DIFF_ST2 0x1000000
#define ISA_DIFF_END2 PMEM_SIZE
void isa_difftest_attach(void) {
  // copy [0x0, 0x7c00) and [0x100000, PMEM_SIZE)
  char *mainargs = guest_to_host(0);
  ref_difftest_memcpy_from_dut(PC_START-IMAGE_START, mainargs, 0x7c00);
  mainargs = (char *)ISA_DIFF_ST2;
  ref_difftest_memcpy_from_dut(PC_START-IMAGE_START+ISA_DIFF_ST2, mainargs, ISA_DIFF_END2 - ISA_DIFF_ST2);
  ref_difftest_setregs(&cpu);
}

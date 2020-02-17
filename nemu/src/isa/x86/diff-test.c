#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 8; i++) {
    if (ref_r->gpr[i]._32 != reg_l(i)) {
      Log("ref_r->gpr[%d] != cpu.gpr[%d]. ref_r->gpr = 0x%x", i, i, ref_r->gpr[i]._32);
      return false;
    }
  }
  if (ref_r->pc != cpu.pc) {
    Log("ref_r->pc != cpu.pc");
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
}

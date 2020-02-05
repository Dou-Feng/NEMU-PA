#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
	printf("eax\t 0x%08x\t %08u\n", cpu.eax, cpu.eax);
	printf("ebx\t 0x%08x\t %08u\n", cpu.ebx, cpu.ebx);
	printf("ecx\t 0x%08x\t %08u\n", cpu.ecx, cpu.ecx);
	printf("edx\t 0x%08x\t %08u\n", cpu.edx, cpu.edx);
	printf("esi\t 0x%08x\t %08u\n", cpu.esi, cpu.esi);
	printf("edi\t 0x%08x\t %08u\n", cpu.edi, cpu.edi);
	printf("ebp\t 0x%08x\t %08u\n", cpu.ebp, cpu.ebp);
	printf("esp\t 0x%08x\t %08u\n", cpu.esp, cpu.esp);
	printf("pc\t 0x%08x\t %08u\n", cpu.pc, cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
	char *p = (char *)s;
	if (p == NULL) {
		*success = 0;
		return 0;
	}
	
	// dismiss the '$'
	if (p[0] == '$') {
		p++;
	}
	*success = true;
	if (strcmp(p, "pc") == 0 || strcmp(p, "eip") == 0) {
		return cpu.pc;
	}
	// find the reg from the list
	for (int i = 0; i < 8; i++) {
		if (strcmp(p, regsl[i]) == 0) {
			return reg_l(i);
		} else if (strcmp(p, regsw[i]) == 0) {
			return reg_w(i);
		} else if (strcmp(p, regsb[i]) == 0) {
			return reg_b(i);
		}
	} 
	*success = false;
	return 0;
}

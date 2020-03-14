#include "proc.h"

#define MAX_NR_PROC 4

void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  switch_boot_pcb();
  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/init");
  // context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/events");
  // context_uload(&pcb[1], "/bin/hello");
 
}

_Context* schedule(_Context *prev) {
  // return prev;
  current->cp = prev;

  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  current = &pcb[0];
  // Log("Schedule! current->as->ptr = 0x%x, current->cp->eip = 0x%x", current->as.ptr, current->cp->eip);
  current->cp->as = &current->as;
  return current->cp;
}

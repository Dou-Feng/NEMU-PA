#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

extern char _end;

void *_brk(const void *addr);
int _execve(const char *path, char *const argv[], char *const envp[]);
int mm_brk(uintptr_t brk, intptr_t increment);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // Log("System call: %d", a[0]);
  switch (a[0]) {
    case SYS_exit: _halt(0); c->GPRx = _execve("/bin/init", (char **const)a[2], (char **const)a[3]); break;
    case SYS_yield: _yield(); c->GPRx = 0; break;
    case SYS_open: c->GPRx = fs_open((char *)a[1], a[2], a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (void*)a[2], a[3]); break;
    case SYS_kill: 
    case SYS_getpid:
    case SYS_close: break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_brk: c->GPRx = mm_brk(a[1], a[2]); break;
    case SYS_fstat:
    case SYS_time:
    case SYS_signal:
    case SYS_execve: c->GPRx = _execve((char *)a[1], (char **const)a[2], (char **const)a[3]); break;
    case SYS_fork:
    case SYS_link:
    case SYS_unlink:
    case SYS_wait:
    case SYS_times:
    case SYS_gettimeofday:
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}

size_t _write(int fildes, const void *buf, size_t nbyte) {
  // for simplicity, don't care fildes
  int ret = 0;
  char *str = (char *)buf;
  for (ret = 0; ret < nbyte; ret++) {
    // don't care the fail case
    _putc(str[ret]);
  }
  return ret;
}

void *_brk(const void *addr) {
  Log("The addr is 0x%x", addr);
  return 0;
}

void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename);
extern PCB *current;

int _execve(const char *path, char *const argv[], char *const envp[]) {
  Log("Execve, path = %s", path);
  context_uload(current, path);
  return 0;
}
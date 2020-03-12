#include "memory.h"
#include "proc.h"

static void *pf = NULL;

extern PCB *current;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  // Log("the brk is 0x%x, max_brk is 0x%x", brk, current->max_brk);
  if (brk > current->max_brk) {
    int size = brk - current->max_brk;
    int page_num = (size + PGSIZE - 1) / PGSIZE;
    Log("Enlarge page size is %d", page_num);
    void *pages = new_page(page_num);
    for (int i = 0; i < page_num; i++) {
      _map(&current->as, (void *)current->max_brk, pages + PGSIZE * i, 0);
      current->max_brk += PGSIZE;
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}

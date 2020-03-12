#include <am.h>
#include <x86.h>
#include <nemu.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }
  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  as->pgsize = PGSIZE;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
    
  }
  // printf("In _protect the as->ptr is 0x%x", as->ptr);
  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}


// 将虚拟地址映射到物理地址中
int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  // printf("In _map:: the va is 0x%x\n", (uint32_t) va);
  // get PDE addr
  uint32_t *dir_addr = (uint32_t*) (((uint32_t) as->ptr & 0xfffff000) + (PDX(va) << 2));
  // printf("The dir_addr is 0x%x\n", dir_addr);
  void *page;
  if ((*dir_addr & PTE_P) == 0) { // 无效的pde
    // allocate a new page
    page = pgalloc_usr(1);
    *dir_addr = (uint32_t) page | PTE_P;
    // printf("*alloced, dir_addr is 0x%x\n", *dir_addr);
  } else { // pde 有效
    page = (void *) *dir_addr;
  }

  uint32_t *page_addr = (uint32_t *) (((uint32_t)page & 0xfffff000) + (PTX(va) << 2));
  if ((*page_addr & PTE_P) == 0) { // 无效的pte
    *page_addr = (((uint32_t)pa & 0xfffff000) | PTE_P); // 变为有效的pte
  } else { // 有效的pte, 那么pt中的pte应该与物理地址对应
    assert(((uint32_t)pa & 0xfffff000) == (*page_addr & 0xfffff000));
  }
  
  // printf("In _map:: the page addr is 0x%x, the page_addr is 0x%x, the *page_addr is 0x%x\n", (uint32_t) page, page_addr, *page_addr);

  printf("_map va -> pa: 0x%x -> 0x%x\n", va, (*page_addr & 0xfffff000) + OFF(va));
  return (*page_addr & 0xfffff000) + OFF(va);
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  _Context *cp = (_Context *) ((int)ustack.end - sizeof(_Context) - sizeof(int) - sizeof(char **) - sizeof(char**));

  cp->eip = (uint32_t) entry;
  cp->cs = 8;

  // init as
  as->pgsize = PGSIZE;
  as->area.start = ustack.start;
  as->area.end = ustack.end;
  // printf("In ucontext:: the ptr is 0x%x\n", as->ptr);
  return cp;
}

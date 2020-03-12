#include "nemu.h"


// 考虑到PTE和PDE都是4字节，所以对于PT和PE索引都需要每4字节移动一次，因此需要用(((uint32_t) vaddr.dir) << 2)来进行索引
// 在最后从PT中得到了page_num之后，需要左移12位
static inline paddr_t page_translate(vaddr_t addr) {
  // Log("The addr is 0x%x", addr);
  vaddr_t_m vaddr = (vaddr_t_m) addr;

  // get PDE
  PDE pde;
  uint32_t dir_addr = (cpu.cr3.val & ~OFF_MASK) | ((vaddr.dir << 2) & OFF_MASK);

  pde.val = paddr_read(dir_addr, 4);
  Assert(pde.present, "The pde.present is invalid, the addr is 0x%x, the cr3's base is 0x%x", addr, cpu.cr3.val);

  // get PTE
  PTE pte;
  uint32_t page_addr = (pde.val & ~OFF_MASK) | ((vaddr.page << 2) & OFF_MASK);

  pte.val = paddr_read(page_addr, 4);
  Assert(pte.present, "The pte.present is invalid, the addr is 0x%x, the cr3's base is 0x%x, the page_addr is 0x%x, the pte is 0x%x", addr, cpu.cr3.val, page_addr, pte.val);

  // get physical address
  uint32_t paddr = (pte.val & ~OFF_MASK) | (vaddr.offset & OFF_MASK);
  return paddr;
}

static inline bool is_cross_boundary(vaddr_t addr, int len) {
  // considered a page is 4K,so if addr / 4k = x, and (addr + len - 1) / 4k = y, and x != y, 
  // then, the result is true.Otherwise the result is false.
  uint32_t x = addr / PAGE_SIZE;
  uint32_t y = (addr + len - 1) / PAGE_SIZE;
  return x != y;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if (!cpu.cr0.paging) {
    return paddr_read(addr, len);
  }
  if (is_cross_boundary(addr, len)) {
    int rest_len = (addr+len) % PAGE_SIZE;
    len -= rest_len;
    paddr_t paddr = page_translate(addr);
    uint32_t res1 = paddr_read(paddr, len);
    
    // read the rest data
    addr += len;
    paddr = page_translate(addr);
    uint32_t res2 = paddr_read(paddr, rest_len);

    // integrate
    return res1 + (res2 << (len << 3));
  } else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (!cpu.cr0.paging) {
    paddr_write(addr, data, len);
    return;
  }
  if (is_cross_boundary(addr, len)) {
    int rest_len = (addr+len) % PAGE_SIZE;
    len -= rest_len;
    
    int mask = 0xff000000 >> ((rest_len-1) << 3);

    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data & ~mask, len);
    
    // write the rest data
    addr += len;
    paddr = page_translate(addr);
    paddr_write(paddr, ((uint32_t)(data & mask)) >> (len << 3), rest_len);
  } else {
    paddr_t paddr = page_translate(addr);
    // Log("addr is 0x%x, paddr is 0x%x", addr, paddr);
    paddr_write(paddr, data, len);
  }
}

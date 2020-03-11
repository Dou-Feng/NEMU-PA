#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define MAX_ELF_Phdr_SIZE 20

static uintptr_t loader(PCB *pcb, const char *filename) {
  // firstly, read ELF header from ramdisk
  Elf_Ehdr elf_header;
  // ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));
  // assert(sizeof(Elf_Ehdr) == elf_header.e_ehsize); 
  
  // use fs_read to load excuteable files
  int fd = fs_open(filename, 0, 0);
  Log("the fd = %d, filename =%s", fd, filename);
  assert(fs_read(fd, &elf_header, sizeof(Elf_Ehdr)) == elf_header.e_ehsize);

  // get program Headers
  // NEMU don't have malloc and free function, so we simply use the array.
  Elf_Phdr elf_pro_header[MAX_ELF_Phdr_SIZE];
  // Elf_Phdr *elf_pro_header = (Elf_Phdr *) malloc(elf_header.e_phentsize * elf_header.e_phnum);
  // ramdisk_read((void*)elf_pro_header, elf_header.e_phoff, elf_header.e_phentsize * elf_header.e_phnum);
  
  // use fs_read to load elf program header
  fs_lseek(fd, elf_header.e_phoff, SEEK_SET);
  fs_read(fd, (void*)elf_pro_header, elf_header.e_phentsize * elf_header.e_phnum);

  // load data through the elf program header 
  for (int i = 0; i < elf_header.e_phnum; i++) {
    // if type is not PT_LOAD, don't care
    if (elf_pro_header[i].p_type != PT_LOAD) continue;

    // copy the ramdisk's sengment to the NEMU's memory
    // ramdisk_read((void*)elf_pro_header[i].p_vaddr, elf_pro_header[i].p_offset, elf_pro_header[i].p_filesz);
    
    // use fd_read to load the segments
    fs_lseek(fd, elf_pro_header[i].p_offset, SEEK_SET);
    fs_read(fd, (void *)elf_pro_header[i].p_vaddr, elf_pro_header[i].p_filesz);

    // assgin the uninitialized data segment to 0
    if (elf_pro_header[i].p_filesz < elf_pro_header[i].p_memsz) {
      memset((void*)(elf_pro_header[i].p_vaddr+elf_pro_header[i].p_filesz), 0, 
        elf_pro_header[i].p_memsz - elf_pro_header[i].p_filesz);
    }
  }
  // // don't forget to free the memory
  // free(elf_pro_header);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  // _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

#include <proc.h>
#include <elf.h>
#include <common.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();

  Elf_Ehdr eh;
  ramdisk_read(&eh, 0, sizeof(eh));

  for (int i = 0; i < eh.e_phnum; i++) {
    printf("Loading ph#%d", i);
    Elf_Phdr ph;
    ramdisk_read(&ph, eh.e_phoff + i*eh.e_phentsize, eh.e_phentsize);
    memset((void*)ph.p_vaddr, 0, ph.p_memsz);
    ramdisk_read((void*)ph.p_vaddr, ph.p_offset, ph.p_filesz);
  }

  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}


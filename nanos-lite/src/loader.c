#include <proc.h>
#include <elf.h>
#include <common.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();

  Elf_Ehdr eh;
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &eh, sizeof(eh));

  assert(eh.e_machine == EXPECT_TYPE);

  for (int i = 0; i < eh.e_phnum; i++) {
    printf("Loading ph#%d\n", i);
    Elf_Phdr ph;
    fs_lseek(fd, eh.e_phoff + i*eh.e_phentsize, SEEK_SET);
    fs_read(fd ,&ph, eh.e_phentsize);
    if (ph.p_type == PT_NULL) continue;
    else if (ph.p_type == PT_LOAD) {
      memset((void*)ph.p_vaddr, 0, ph.p_memsz);

      fs_lseek(fd, ph.p_offset, SEEK_SET);
      fs_read(fd, (void*)ph.p_vaddr, ph.p_filesz);
    }
  }

  fs_close(fd);

  return eh.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}


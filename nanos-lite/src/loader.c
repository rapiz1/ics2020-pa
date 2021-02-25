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
  if (fd < 0) return 0;

  fs_read(fd, &eh, sizeof(eh));

  assert(eh.e_machine == EXPECT_TYPE);

  for (int i = 0; i < eh.e_phnum; i++) {
    printf("Loading ph#%d\n", i);
    Elf_Phdr ph;
    fs_lseek(fd, eh.e_phoff + i*eh.e_phentsize, SEEK_SET);
    fs_read(fd ,&ph, eh.e_phentsize);
    if (ph.p_type == PT_NULL) continue;
    else if (ph.p_type == PT_LOAD) {
      void* new_page(size_t nr_page);

      int pg_n = ROUNDUP(ph.p_memsz, PGSIZE)/PGSIZE;
      assert(pg_n == 1);
      void *pg = new_page(pg_n);
      memset(pg + GET_OFFSET(ph.p_paddr), 0, ph.p_memsz);

      fs_lseek(fd, ph.p_offset, SEEK_SET);
      fs_read(fd, pg + GET_OFFSET(ph.p_vaddr), ph.p_filesz);

      for (int i = 0; i < pg_n; i++)
        map(&pcb->as, (void*)ph.p_vaddr + i*PGSIZE, pg + i*PGSIZE, 0);
    }
  }

  fs_close(fd);

  return eh.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}


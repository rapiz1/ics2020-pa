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
    Elf_Phdr ph;
    fs_lseek(fd, eh.e_phoff + i*eh.e_phentsize, SEEK_SET);
    fs_read(fd ,&ph, eh.e_phentsize);
    if (ph.p_type == PT_NULL) continue;
    else if (ph.p_type == PT_LOAD) {
      printf("Loading ph#%d at 0x%x\n", i, ph.p_vaddr);
      void* new_page(size_t nr_page);
      void *lookup_map(AddrSpace *as, void *va);
      for (int i = 0; i < ph.p_memsz;) {
        void *vaddr = (void*)ph.p_vaddr + i;
        void *paddr_base = new_page(1);
        memset(paddr_base, 0, PGSIZE);
        map(&pcb->as, vaddr, paddr_base, 0);
        fs_lseek(fd, ph.p_offset + i, SEEK_SET);

        int sz = 0;
        if ((uint32_t)vaddr == ROUNDUP(vaddr, PGSIZE)) sz = PGSIZE;
        else sz = ROUNDUP(vaddr, PGSIZE) - (uint32_t)vaddr;

        fs_read(fd, paddr_base + GET_OFFSET(vaddr), max(0, min(sz, ph.p_filesz - i)));
        i += sz;
      }
/*
      int pg_n = ROUNDUP(ph.p_memsz, PGSIZE)/PGSIZE;
      void *pg = new_page(pg_n);
      memset(pg + GET_OFFSET(ph.p_paddr), 0, ph.p_memsz);

      fs_lseek(fd, ph.p_offset, SEEK_SET);
      fs_read(fd, pg + GET_OFFSET(ph.p_vaddr), ph.p_filesz);

      for (int i = 0; i < pg_n; i++)
        map(&pcb->as, (void*)ph.p_vaddr + i*PGSIZE, pg + i*PGSIZE, 0);
        */
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


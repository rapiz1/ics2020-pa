#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  printf("start creating page tables\n");
  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    printf("kenerl seg [%d, %d)\n", va, segments[i].end);
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  void lookup(AddrSpace *as, void *va);
  lookup(&kas, (void*)0x00101452);
  printf("table created\n");
  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  PTE *updir = (PTE*)as->ptr;
  PTE *pde = &updir[GET_DIR(va)];
  if (pde->present == 0) {
    pde->present = 1;
    pde->page_frame_address = ((uintptr_t)pgalloc_usr(PGSIZE) >> 12);
  }

  PTE *pt = (PTE*)(pde->page_frame_address<<12);
  PTE *pte = &pt[GET_PAGE(va)];
  pte->present = 1;
  pte->page_frame_address = (uintptr_t)pa >> 12;
}

void lookup(AddrSpace *as, void *va) {
  PTE *updir = (PTE*)as->ptr;
  PTE *pde = &updir[GET_DIR(va)];
  assert(pde->present);

  PTE *pt = (PTE*)(pde->page_frame_address<<12);
  PTE *pte = &pt[GET_PAGE(va)];
  assert(pte->present);
  printf("lookup %d->%d\n", va, pte->page_frame_address<<12);
}

Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *cp = (Context*)kstack.end - 1;
  cp->eip = (uint32_t)entry;
  cp->esp = (uint32_t)&cp->irq;
  cp->cs = 8;
  return cp;
}

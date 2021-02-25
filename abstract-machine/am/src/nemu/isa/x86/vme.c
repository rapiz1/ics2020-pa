#include <am.h>
#include <nemu.h>
#include <klib.h>

#define Log(format, ...) \
  printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ## __VA_ARGS__)


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

  Log("Start creating page tables");
  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    Log("Kenerl seg [%x, %x)\n", va, segments[i].end);
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  void lookup(AddrSpace *as, void *va);
  lookup(&kas, (void*)0x00101452);
  lookup(&kas, (void*)3001296);
  Log("Table created");
  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;
  Log("Kernel pd at 0x%x", kas.ptr);

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
  printf("new pd created at 0x%x\n", updir);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
  //printf("cr3 saved 0x%x\n", c->cr3);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    //printf("cr3 switch to 0x%x\n", c->cr3);
    set_cr3(c->cr3);
  }// else printf("cr3 in context NULL, not changed\n");
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  //assert(GET_OFFSET(va) == GET_OFFSET(pa));
  PTE *updir = (PTE*)as->ptr;
  PTE *pde = &updir[GET_DIR(va)];
  if (pde->present == 0) {
    pde->present = 1;
    pde->page_frame_address = ((uintptr_t)pgalloc_usr(PGSIZE) >> 12);
  }

  PTE *pt = (PTE*)(pde->page_frame_address<<12);
  PTE *pte = &pt[GET_PAGE(va)];
  assert(!pte->present);
  pte->present = 1;
  pte->page_frame_address = (uintptr_t)pa >> 12;
}


void *lookup_map(AddrSpace *as, void *va) {
  // This is only for debugging
  PTE *updir = (PTE*)as->ptr;
  PTE *pde = &updir[GET_DIR(va)];
  if (!pde->present) return NULL;

  PTE *pt = (PTE*)(pde->page_frame_address<<12);
  PTE *pte = &pt[GET_PAGE(va)];
  if (!pte->present) return NULL;
  return (void*)((pte->page_frame_address)<<12);
}

void lookup(AddrSpace *as, void *va) {
  // This is only for debugging
  PTE *updir = (PTE*)as->ptr;
  PTE *pde = &updir[GET_DIR(va)];
  assert(pde->present);

  PTE *pt = (PTE*)(pde->page_frame_address<<12);
  PTE *pte = &pt[GET_PAGE(va)];
  assert(pte->present);
  void *page_base = (void*)((pte->page_frame_address)<<12);
  printf("lookup %p->%p\n", va, (uint32_t)page_base | GET_OFFSET(va));
}

Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *cp = (Context*)kstack.end - 1;
  cp->eip = (uint32_t)entry;
  cp->esp = (uint32_t)&cp->irq;
  cp->cs = 8;
  cp->cr3 = as->ptr;
  cp->eflags = 0x2;
  return cp;
}

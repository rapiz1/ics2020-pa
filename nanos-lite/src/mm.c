#include <memory.h>
#include <common.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *old_pf = pf;
  pf += nr_page*PGSIZE;
  Log("new_page at %p", old_pf);
  return old_pf;
}

static inline void* pg_alloc(int n) {
  int sz = ROUNDUP(n, PGSIZE);
  void* pg = new_page(sz/PGSIZE);
  memset(pg, 0, sz);
  return pg;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}

#include <memory.h>
#include <common.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *old_pf = pf;
  pf += nr_page*PGSIZE;
  assert(pf < heap.end);
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
  extern PCB *current;
  if (current->max_brk == 0) {
    // in crt0.c, the program always call sbrk(0)
    current->max_brk = brk;
    if (ROUNDUP(brk, PGSIZE) == brk) {
      void *pg = new_page(1);
      map(&current->as, (void*)brk, pg, 0);
    }
  } else if (current->max_brk < brk) {
    for (void *start = (void*)ROUNDUP(current->max_brk, PGSIZE), *end = (void*)ROUNDUP(brk, PGSIZE); start != end; start += PGSIZE) {
      void *pg = new_page(1);
      map(&current->as, start, pg, 0);
    }
    current->max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}

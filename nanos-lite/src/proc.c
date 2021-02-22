#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() { current = &pcb_boot; }

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%d' for the %dth time!, at %d",
        (uintptr_t)arg, j, &arg);
    j++;
    yield();
  }
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Context *cp = kcontext(RANGE(pcb, pcb + 1), entry, arg);
  pcb->cp = cp;
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)1);
  context_kload(&pcb[1], hello_fun, (void*)99);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  /*
  extern void naive_uload(PCB *pcb, const char *filename);
  naive_uload(NULL, "/bin/nterm");
  */
}

Context *schedule(Context *prev) {
  //Log("schedule called");
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  //current = &pcb[0];
  if (current == &pcb_boot)
    current = &pcb[0];
  else
    current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}

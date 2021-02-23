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

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  extern uintptr_t loader(PCB *pcb, const char *filename);

  uintptr_t entry = loader(pcb, filename);

  Context *cp = ucontext(NULL, RANGE(pcb, pcb+1), (void(*)())entry);
  pcb->cp = cp;

  //void *st = heap.end, *strp = heap.end;
  void *st = new_page(8), *strp = st;

  int envc = 0;
  for(int i = 0; envp && envp[i]; i++) {
    envc++;
    int n = strlen(envp[i]) + 1;
    st -= n;
    strcpy(st, envp[i]);
  }

  int argc = 0;
  for(int i = 0; argv && argv[i]; i++) {
    argc++;
    int n = strlen(argv[i]) + 1;
    st -= n;
    strcpy(st, argv[i]);
  }

  st -= sizeof(char*)*(envc+1);
  char **user_envp = st;
  for (int i = 0; i < envc; i++) {
    int n = strlen(envp[i]) + 1;
    strp -= n;
    user_envp[i] = strp;
  }
  user_envp[envc] = NULL;

  st -= sizeof(char*)*(argc+1);
  char **user_argv = st;
  for (int i = 0; i < argc; i++) {
    int n = strlen(argv[i]) + 1;
    strp -= n;
    user_argv[i] = strp;
  }
  user_argv[argc] = NULL;

  st -= sizeof(uint32_t);
  *(uint32_t*)st = argc;

  cp->GPRx = (uint32_t)st;
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Context *cp = kcontext(RANGE(pcb, pcb + 1), entry, arg);
  pcb->cp = cp;
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)1);
  char *argv[] = {"HELLO FROM NANOS-LITE", NULL};
  char *envp[] = {"HELLO=NANOS-LITE", "AUTHOR=rapiz", NULL};
  context_uload(&pcb[1], "/bin/nterm", argv, envp);
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

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = (int)args;
  char **argv = (char**)&args + 1;
  environ = (char**)&args + 1 + argc + 1;
  exit(main(argc, argv, environ));
  assert(0);
}

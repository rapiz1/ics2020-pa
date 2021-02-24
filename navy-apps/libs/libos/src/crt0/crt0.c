#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  puts("======CRT0========");
  sbrk(0);
  int argc = (int)args;
  char **argv = (char**)&args + 1;
  environ = (char**)&args + 1 + argc + 1;
  for (int i = 0; i < argc; i++)
    printf("argv[%d]=%s\n", i, argv[i]);
  for (int i = 0; environ[i]; i++)
    printf("env %s\n", environ[i]);
  puts("=====CRT0 END=====");
  puts("======START=======");
  exit(main(argc, argv, environ));
  assert(0);
}

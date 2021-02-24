#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  /*
  puts("======CRT0========");
  for (int i = 0; i < argc; i++)
    printf("argv[%d]=%s\n", i, argv[i]);
  for (int i = 0; environ[i]; i++)
    printf("env %s\n", environ[i]);
  puts("=====CRT0 END=====");
  puts("======START=======");
  */
  int argc = (int)args;
  char **argv = (char**)&args + 1;
  environ = (char**)&args + 1 + argc + 1;
  exit(main(argc, argv, environ));
  assert(0);
}

#include <stdio.h>
#include "monitor/debug/expr.h"
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

void test_expr() {
  FILE* f = fopen("tools/gen-expr/input", "r");
  if (f == NULL) {
    puts("open failed");
    return;
  }
  int r;
  char s[1<<16];
  int i = 0;
  while (fscanf(f, "%d %s", &r, s) != EOF) {
    puts(s);
    bool success = false;
    int mr = expr(s, &success);
    Assert(mr == r, "%s, expect %d, got %d", s, r, mr);
    printf("PASS %d\n", i++);
  }
  fclose(f);
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  test_expr();

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}

#include <stdio.h>
#include "monitor/debug/expr.h"
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

void test_expr() {
  FILE* f = fopen("../tools/gen-expr/input", "r");
  int r;
  char s[1<<16];
  while (scanf("%d %s", &r, s) != EOF) {
    bool success = false;
    int mr = expr(s, &success);
    assert(mr == r);
    printf("PASS");
  }
  fclose(f);
}

int main(int argc, char *argv[]) {
  test_expr();
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}

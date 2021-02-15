#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int expr_buf_pos;

static void gen_num() {
  expr_buf_pos += sprintf(buf + expr_buf_pos, "%d", rand()%100+1);
}

static void gen_rand_op() {
  switch(rand()%4) {
    case 0: buf[expr_buf_pos++] = '+'; break;
    case 1: buf[expr_buf_pos++] = '-'; break;
    case 2: buf[expr_buf_pos++] = '*'; break;
    case 3: buf[expr_buf_pos++] = '/'; break;
  }
}

static inline void gen_rand_expr() {
  if (expr_buf_pos > 10000) {
    gen_num();
    return;
  }
  switch(rand()%3) {
    case 0: gen_num(); break;
    case 1: buf[expr_buf_pos++] = '('; gen_rand_expr(); buf[expr_buf_pos++] = ')'; break;
    case 2: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    expr_buf_pos = 0;
    gen_rand_expr();
    buf[expr_buf_pos++] = 0;

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

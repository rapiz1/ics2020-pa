#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

  char exp[1024];
  int old_val;
} WP;

bool check_watchpoints();
void show_watchpoints();
int add_watchpoint(char *exp);
int delete_watchpoints_byNo(int no);

#endif

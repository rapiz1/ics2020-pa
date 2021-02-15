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

bool check_watchpoint();
void show_watchpoints();

#endif

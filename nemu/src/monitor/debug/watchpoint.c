#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  WP* cur = free_;
  if (cur == NULL) {
    panic("can't alloc new wp");
  }
  free_ = free_->next;

  cur->next = head;
  head = cur;
  return cur;
}

void free_wp(WP *wp) {
  for (WP *cur = head, *prev = NULL; cur != NULL; prev = cur, cur = cur->next) {
    if (cur == wp && prev != NULL) {
      prev->next = cur->next;
      cur->next = NULL;
      break;
    }
  }

  wp->next = free_;
  free_ = wp;
  if (head == wp) {
    head = NULL;
  }
}

// Return if should stop
bool check_watchpoints() {
  bool stop = false;
  for (WP *cur = head; cur; cur = cur->next) {
    bool success = false;
    int new_val = expr(cur->exp, &success);
    if (success && new_val != cur->old_val) {
      Log("Watchpoint#%d: %s %d->%d", cur->NO, cur->exp, cur->old_val, new_val);
      cur->old_val = new_val;
      stop = true;
    }
  }
  return stop;
}

// Show all watchpoints
void show_watchpoints() {
  for (WP* cur = head; cur; cur = cur->next) {
    printf("Watchpoint#%d: %s %d\n", cur->NO, cur->exp, cur->old_val);
  }
}

int add_watchpoint(char *exp) {
  bool success = false;
  int old_val = expr(exp, &success);
  if (!success) {
    puts("failed to calculate the expression");
    return -1;
  }

  WP* wp = new_wp();
  strcpy(wp->exp, exp);
  wp->old_val = old_val;
  return 0;
}

int delete_watchpoints_byNo(int no) {
  for (WP* cur = head; cur; cur = cur->next) {
    if (cur->NO == no) {
      free_wp(cur);
      Log("deleted Watchpoint#%d", no);
      return 0;
    }
  }
  Log("Not found Watchpoint#%d", no);
  return -1;
}

#include <common.h>
#include "syscall.h"
#include <errno.h>
#include <fs.h>

static void sys_exit(Context *c) {
  halt(0);
}

static void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

static void sys_open(Context *c) {
  char *pathname = (char*)c->GPR2;
  c->GPR2 = fs_open(pathname, 0, 0);
}

static void sys_close(Context *c) {
  int fd = c->GPR2;
  c->GPR2 = fs_close(fd);
}

static void sys_write(Context *c) {
  int fd = c->GPR2;
  char *buf = (char*)c->GPR3;
  size_t len = c->GPR4;
  if (fd == 0 || fd == 1) {
    for (int i = 0; i < len; i++)
      putch(buf[i]);
    c->GPRx = len;
  } else {
    c->GPRx = fs_write(fd, buf, len);
  }
}

static void sys_brk(Context *c) {
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: sys_exit(c); break;
    case SYS_yield: sys_yield(c); break;
    case SYS_open: sys_open(c); break;
    case SYS_write: sys_write(c); break;
    case SYS_close: sys_close(c); break;
    case SYS_brk: sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

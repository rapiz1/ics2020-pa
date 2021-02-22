#include <common.h>
#include "syscall.h"
#include <errno.h>
#include <fs.h>
#include <sys/time.h>
#include <proc.h>

extern void naive_uload(PCB *pcb, const char *filename);

static void sys_exit(Context *c) {
  naive_uload(NULL, "/bin/nterm");
}

static void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

static void sys_open(Context *c) {
  char *pathname = (char*)c->GPR2;
  c->GPRx = fs_open(pathname, 0, 0);
}

static void sys_read(Context *c) {
  int fd = c->GPR2;
  char *buf = (char*)c->GPR3;
  int count = c->GPR4;
  c->GPRx = fs_read(fd, buf, count);
}

static void sys_write(Context *c) {
  int fd = c->GPR2;
  char *buf = (char*)c->GPR3;
  size_t len = c->GPR4;
  c->GPRx = fs_write(fd, buf, len);
}

static void sys_close(Context *c) {
  int fd = c->GPR2;
  c->GPRx = fs_close(fd);
}

static void sys_lseek(Context *c) {
  int fd = c->GPR2;
  int offset = c->GPR3;
  int whence = c->GPR4;
  c->GPRx = fs_lseek(fd, offset, whence);
}

static void sys_brk(Context *c) {
  c->GPRx = 0;
}

static void sys_execve(Context *c) {
  c->GPRx = 0;
  char *pathname = (char*)c->GPR2;
  //char **args = (char**)c->GPR3;
  char **env = (char**)c->GPR4;
  if (env != NULL)
    for (int i = 0; env[i] != NULL; i++) {
      if (!strncmp(env[i], "PATH", 4)) {
        char buf[1024] = {};
        strcat(buf, env[i] + 5);
        strcat(buf, "/");
        strcat(buf, pathname);
        printf("try to load %s\n", buf);
        naive_uload(NULL, buf);
      }
    }
  naive_uload(NULL, pathname);
}

static void sys_gettimeofday(Context *c) {
  struct timeval *tv = (struct timeval *)c->GPR2;
  //struct timezone *tz = (struct timezone *)c->GPR3;
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us % 1000000;
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: sys_exit(c); break;
    case SYS_yield: sys_yield(c); break;
    case SYS_open: sys_open(c); break;
    case SYS_read: sys_read(c); break;
    case SYS_write: sys_write(c); break;
    case SYS_close: sys_close(c); break;
    case SYS_lseek: sys_lseek(c); break;
    case SYS_brk: sys_brk(c); break;
    case SYS_execve: sys_execve(c); break;
    case SYS_gettimeofday: sys_gettimeofday(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

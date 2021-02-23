#include <common.h>
#include "syscall.h"
#include <errno.h>
#include <fs.h>
#include <sys/time.h>
#include <proc.h>

static void sys_exit(Context *c) {
  halt(0);
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
  char *filename = (char*)c->GPR2;
  char **argv = (char**)c->GPR3;
  char **env = (char**)c->GPR4;

#define STRLEN(x) (sizeof(x)-1)
  int fd = fs_open(filename, 0, 0);
  if (fd >= 0) {
    fs_close(fd);
  } else {
    char *path = NULL;
    for (int i = 0; env && env[i]; i++) {
      if (strncmp(env[i], "PATH", STRLEN("PATH"))) continue;
      path = env[i];
      break;
    }
    if (!path) goto not_found;

    static char concat_filename[1024];

    while (*path) {
      int n = 0;
      while (*path && *path == ':') path++;
      while (*path && *path != ':')
        concat_filename[n++] = *path++;
      if (!n) break;

      if (concat_filename[n-1] != '/')
        concat_filename[n++] = '/';
      concat_filename[n++] = '\0';

      strcat(concat_filename, filename);

      fd = fs_open(concat_filename, 0, 0);
      if (fd >= 0) {
        fs_close(fd);
        break;
      }
    }

    if (fd < 0)
      goto not_found;
  }

  extern void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
  context_uload(current, filename, argv, env);

  // Note that we don't actually switch to boot_pcb.
  // The scheduler will ensure that.
  extern void switch_boot_pcb();
  switch_boot_pcb();
  yield();
  not_found:
  c->GPRx = -ENOENT;
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

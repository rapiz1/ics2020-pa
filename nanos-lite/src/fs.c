#include <fs.h>
#include <common.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

static int open_offset[LENGTH(file_table)];

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = cfg.width * cfg.height;
}

int fs_open(const char *pathname, int flags, int mode) {
  //Log("opening %s", pathname);
  int n = LENGTH(file_table);
  for (int i = 0; i < n; i++) {
    if (!strcmp(file_table[i].name, pathname)) {
      open_offset[i] = 0;
      if (file_table[i].read == NULL) {
        file_table[i].read = ramdisk_read;
        file_table[i].write = ramdisk_write;
      }
      //Log("return fd %d", i);
      return i;
    }
  }
  Log("can't find %s", pathname);
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
  Finfo *f = file_table + fd;
  int ret = f->read(buf, file_table[fd].disk_offset + open_offset[fd], len);
  open_offset[fd] += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  Finfo *f = file_table + fd;
  int ret = f->write(buf, file_table[fd].disk_offset + open_offset[fd], len);
  open_offset[fd] += ret;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd <= FD_STDERR)
    panic("fs_lseek on invalid fd %d", fd);

  int pos = open_offset[fd];
  if (whence == SEEK_SET)
    pos = offset;
  else if (whence == SEEK_CUR)
    pos = pos + offset;
  else if (whence == SEEK_END)
    pos = file_table[fd].size;
  else panic("invalid whence on fs_lseek");

  return open_offset[fd] = pos;
}

int fs_close(int fd) {
  open_offset[fd] = 0;
  if (file_table[fd].read == ramdisk_read) {
    file_table[fd].read = invalid_read;
    file_table[fd].write = invalid_write;
  }
  return 0;
}

#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

static int open_offset[LENGTH(file_table)];

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
  int n = LENGTH(file_table);
  for (int i = 0; i < n; i++) {
    if (!strcmp(file_table[i].name, pathname)) {
      open_offset[i] = 0;
      //file_table[i].read = fs_read;
      //file_table[i].write = fs_write;
      return i;
    }
  }
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (fd <= FD_STDERR)
    panic("fs_read on builtin fd");
  int ret = ramdisk_read(buf, file_table[fd].disk_offset + open_offset[fd], len);
  open_offset[fd] += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd <= FD_STDERR)
    panic("fs_write on builtin fd");
  int ret = ramdisk_write(buf, file_table[fd].disk_offset + open_offset[fd], len);
  open_offset[fd] += ret;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd <= FD_STDERR)
    panic("fs_lseek on invalid fd");

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
  if (fd <= FD_STDERR)
    panic("fs_close on invalid fd");
  open_offset[fd] = 0;
  //file_table[fd].read = invalid_read;
  //file_table[fd].write = invalid_write;
  return 0;
}

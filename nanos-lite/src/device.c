#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; i++)
    putch(((char*)buf)[i]);
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  //Log("events_read called");
  int has_kbd  = io_read(AM_INPUT_CONFIG).present;
  if (!has_kbd)
    return 0;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;

  size_t ret = 0;
  if (ev.keydown) ret = snprintf(buf, len, "kd %s", keyname[ev.keycode]);
  else ret = snprintf(buf, len, "ku %s", keyname[ev.keycode]);
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int w = cfg.width, h = cfg.height;
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d\n", w, h);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int w = cfg.width;

  int x = offset % w, y = offset / w;
  io_write(AM_GPU_FBDRAW, x, y, (void*)buf, len, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

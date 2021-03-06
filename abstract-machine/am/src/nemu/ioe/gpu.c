#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t raw_value = inl(VGACTL_ADDR);
  uint32_t w = raw_value>>16, h = raw_value&((1<<16)-1);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w*h 
  };
}

void __am_gpu_init() {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  int w = cfg.width; 
  int h = cfg.height;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  //for (int i = 0; i < w * h; i ++) fb[i] = i;
  for (int i = 0; i < h; i++) for (int j = 0; j < w; j++) {
    fb[i*w + j] = i^j;
  }
  outl(SYNC_ADDR, 1);
}


void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int i = 0; i < ctl->h; i++) for (int j = 0; j < ctl->w; j++) {
    int x = ctl->x + j, y = ctl->y + i;
    fb[y*cfg.width + x] = ((uint32_t *)ctl->pixels)[i*ctl->w + j];
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

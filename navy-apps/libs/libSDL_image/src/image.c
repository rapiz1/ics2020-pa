#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  char *buf = malloc(size);

  fseek(f, 0, SEEK_SET);
  fread(buf, size, size, f);
  SDL_Surface *sf = STBIMG_LoadFromMemory(buf, size);

  free(buf);
  fclose(f);
  return sf;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}

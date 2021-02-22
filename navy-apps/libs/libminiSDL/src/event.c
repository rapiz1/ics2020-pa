#include <NDL.h>
#include <SDL.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  if (event == NULL) return 1;
  static char buf[64];
  int len = NDL_PollEvent(buf, 64);
  if (!len) return 0;
  puts(buf);
  if (buf[0] == 'k') {
    int keycode;
    sscanf(buf+2, "%d", &keycode);
    event->key.keysym.sym == keycode;
    if (buf[1] == 'd')
      event->type = SDL_KEYDOWN;
    else if (buf[1] == 'u')
      event->type = SDL_KEYUP;
    else {
      fprintf(stderr, "unknown event");
      assert(false);
    }
  }
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  if (event == NULL) return 1;
  int ret = 0;
  do {
    ret = SDL_PollEvent(event);
  } while (ret == 0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}

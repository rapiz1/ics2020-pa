#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

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
    int keycode = -1;
    for (int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++) {
      if (!strcmp(buf+3, keyname[i])) {
        keycode = i;
        printf("found keycode %d\n", i);
        break;
      }
      //printf("%s == %s", buf+3, keyname[i]);
    }
    assert(keycode != -1);

    event->key.keysym.sym = keycode;
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

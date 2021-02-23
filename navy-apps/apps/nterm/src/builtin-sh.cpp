#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner(int argc, char** argv) {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
  sh_printf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++)
    sh_printf("argv[%d]: %s\n", i, argv[i]);
  for (int i = 0; environ[i] != NULL; i++)
    sh_printf("%s\n", environ[i]);

}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char buf[1024];
  sscanf(cmd, "%s", buf);
  execve(buf, NULL, environ);
}

void builtin_sh_run(int argc, char** argv) {
  sh_banner(argc, argv);
  sh_prompt();
  //while (1) ;

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}

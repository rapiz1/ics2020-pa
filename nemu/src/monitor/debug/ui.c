#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/vaddr.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cpu_exec(1);
    return 0;
  }
  cpu_exec(atoi(args));
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cmd_help(NULL);
    return -1;
  }
  if (!strcmp(arg, "w")) {
    show_watchpoints();
  } else if (!strcmp(arg, "r")) {
    isa_reg_display();
  } else {
    cmd_help(NULL);
    return -1;
  }
  return 0;
}

static int cmd_p(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cmd_help(NULL);
    return -1;
  }
  bool success = false;
  int tmp = expr(arg, &success);
  if (success)
    printf("%d\n", tmp);
  return success;
}

static int cmd_x(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cmd_help(NULL);
    return -1;
  }
  int n = atoi(arg);

  arg = strtok(NULL, " ");
  bool success = false;
  vaddr_t addr = expr(arg, &success);
  if (success == false) {
    Log("failed to calculate the expression");
    return -1;
  }

  printf("0x%08x:", addr);
  for (int i = 0; i < n; i++) {
    int val = vaddr_read(addr+i, 1);
    printf("%02x ", val);
  }
  putchar('\n');
  return 0;
}

static int cmd_w(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cmd_help(NULL);
    return -1;
  }

  return add_watchpoint(arg);
}

static int cmd_d(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    cmd_help(NULL);
    return -1;
  }

  return delete_watchpoints_byNo(atoi(arg));
}


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "si [N] Step", cmd_si},
  { "info", "\tinfo w Show watchpoints\n\tinfo r Show registers", cmd_info},
  { "p", "p EXPR Calc a expression", cmd_p},
  { "x", "x N EXPR Examine the memory", cmd_x},
  { "w", "w EXPR Set a watchpoint", cmd_w},
  { "d", "d N Remove a watchpoint", cmd_d},

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

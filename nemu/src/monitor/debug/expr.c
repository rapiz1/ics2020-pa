#include <isa.h>
#include <stdbool.h>
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_AND, TK_OR,
  TK_INT,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"[0-9]+", TK_INT},
  {"\\(", '('},
  {"\\)", ')'},
};

static int level[512] = {
  [0] = 256,
  [TK_OR] = 12,
  [TK_AND] = 11,
  [TK_EQ] = 7, 
  [TK_NEQ] = 7,
  ['+'] = 4,
  ['-'] = 4,
  ['*'] = 3,
  ['/'] = 3,
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (rules[i].token_type == TK_NOTYPE) {
          break;
        }

        Token t = {
          .type = rules[i].token_type
        };
        memset(t.str, 0, sizeof(t.str));

        switch (rules[i].token_type) {
          case TK_INT:
          if (substr_len > 32) {
            panic("substr too long");
          }
          for (int j = position - substr_len, k = 0; j < position; j++, k++)
            t.str[k] = e[j];
          break;
          default: break;
        }
        tokens[nr_token++] = t;

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_surrounding(int l, int r) {
  if (tokens[l].type != '(') return false;
  int c = 0;
  for (int i = l; i < r; i++) {
    if (tokens[i].type == '(') c++;
    else if (tokens[i].type == ')') c--;
    if (c == 0) return false;
  }
  return c == 1 && tokens[r].type == ')';
}

bool check_brace_balance() {
  int c = 0;
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '(') c++;
    else if (tokens[i].type == ')') c--;
    if (c < 0) {
      return false;
    }
  }
  return c == 0;
}

int eval(int l, int r, bool *success) {
  Log("process %d, %d", l, r);
  if (l > r) {
    panic("eval failed");
  }
  else if (l == r) {
    *success = true;
    return atoi(tokens[l].str);
  } else if (check_surrounding(l, r)) {
    return eval(l+1, r-1, success);
  } else {
    int pcount = 0;
    int main_op = 0;
    int main_op_pos = -1;
    int now_type;
    for (int i = l; i <= r; i++) {
      switch (tokens[i].type)
      {
      case '(':
        pcount++;
        break;
      case ')':
        pcount--;
        break;
      default:
        now_type = tokens[i].type;
        if (pcount == 0 && level[now_type] != 0) {
          if (level[now_type] <= level[main_op]) {
            main_op = now_type;
            main_op_pos = i;
          }
        }
        break;
      }
    }
    Assert(main_op_pos != -1, "can't find the main operator in %d, %d", l, r);
    Log("main op %c", main_op);
    int ret = 0;
    bool ls, rs;
    int lv = eval(l, main_op_pos - 1, &ls), rv = eval(main_op_pos + 1, r, &rs);
    *success = ls && rs;
    switch (main_op)
    {
    case '+': ret = lv + rv; break;
    case '-': ret = lv - rv; break;
    case '/': ret = lv / rv; break;
    case '*': ret = lv * rv; break;
    default:
      break;
    }
    return ret;
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  if (!check_brace_balance()) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  return eval(0, nr_token-1, success);
}

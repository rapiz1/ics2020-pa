#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int _print_ch(char **out, char ch) {
  if (out == NULL)
    putch(ch);
  else {
    **out = ch;
    (*out)++;
  }
  return 1;
}

int _print_str(char **out, char *s) {
  int c = 0;
  while (*s)
    c += _print_ch(out, *s++);
  return c;
}

int _print_digit(char **out, long long x) {
  static char buf[sizeof(int)*8];
  int buf_len = sizeof(int)*8;
  int digit_len = 0, neg = 0; 

  if (x < 0) {
    neg = 1;
    x = -x;
  }
  for (int i = 0; i < buf_len; i++) {
    buf[i] = x%10 + '0';
    digit_len++;
    x /= 10;
    if (x == 0) {
      buf[i+1] = '\0';
      break;
    }
  }
  if (neg)
    _print_ch(out, '-');
  for (int i = digit_len - 1; i >= 0; i--)
    _print_ch(out, buf[i]);
  return digit_len + neg;
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int n = strlen(fmt);
  char *old_out = out;

  for (int i = 0; i < n; i++) {
    char c = fmt[i], type;
    switch (c) {
    case '%':
      if (i+1 >= n)
        assert(0);
      type = fmt[++i];
      if (type == 's') {
        char *s = va_arg(ap, char*);
        _print_str(&out, s);
      } else if (type == 'd') {
        int x = va_arg(ap, int);
        _print_digit(&out, x);
      }
      break;
    default:
      _print_ch(&out, c);
      break;
    }
  }
  _print_ch(&out, '\0');
  return out - old_out - 1;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif

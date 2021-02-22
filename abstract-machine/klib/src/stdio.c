#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int _print_ch(char **out, char ch) {
  if (out == NULL || *out == NULL) {
    if (ch != '\0')
      putch(ch);
  }
  else {
    **out = ch;
    (*out)++;
  }
  return 1;
}

int _print_n_str(char **out, const char *s, size_t n) {
  if (n == 0) return 0;
  int c = 0;
  while (*s) {
    c += _print_ch(out, *s++);
    n--;
    if (n == 0) break;
  }
  return c;
}

int _print_n_digit(char **out, long long x, size_t n) {
  if (n == 0) return 0;
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
  if (neg) {
    _print_ch(out, '-');
    n--;
    if (n == 0) goto out;
  }
  for (int i = digit_len - 1; i >= 0; i--) {
    _print_ch(out, buf[i]);
    n--;
    if (n == 0) goto out;
  }
  out:
  return digit_len + neg;
}

int puts(const char* s) {
  _print_n_str(NULL, s, -1);
  _print_ch(NULL, '\n');
  return 0;
}

int printf(const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int ret = vsprintf(NULL, fmt, va);
  va_end(va);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, -1, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int fmt_n = strlen(fmt);
  char *old_out = out;

  for (int i = 0; i < fmt_n; i++) {
    char c = fmt[i], type;
    switch (c) {
    case '%':
      if (i+1 >= fmt_n)
        assert(0);
      type = fmt[++i];
      if (type == 's') {
        char *s = va_arg(ap, char*);
        n -= _print_n_str(&out, s, n);
      } else if (type == 'd' || type == 'p') {
        int x = va_arg(ap, int);
        n -= _print_n_digit(&out, x, n);
      }
      break;
    default:
      _print_ch(&out, c);
      n--;
      break;
    }
    if (!n) goto out;
  }
  _print_ch(&out, '\0');
  out:
  return out - old_out - 1;
}

#endif

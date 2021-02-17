#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

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
        while (*s)
          *out++ = *s++;
      } else if (type == 'd') {
        char buf[sizeof(int)*8];
        int buf_len = sizeof(int)*8;
        long long x = va_arg(ap, int), digit_len = 0, neg = 0; 
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
          *out++ = '-';
        for (int i = digit_len - 1; i >= 0; i--)
          *out++ = buf[i];
      }
      break;
    default:
      *out++ = c;
      break;
    }
  }
  *out = '\0';
  return out - old_out;
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

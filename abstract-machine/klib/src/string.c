#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int n = 0;
  for (;*s; s++, n++);
  return n;
}

char *strcpy(char* dst,const char* src) {
  int i;
  for (i = 0; src[i]; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  int i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  for (;i < n; i++) dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  int n = strlen(dst);
  int m = strlen(src);
  for (int i = n; i <= n+m; i++)
    dst[i] = src[i-n];
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  unsigned char c1, c2;
  do {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  unsigned char c1, c2;
  int i = 0;
  do {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
      i++;
  } while (c1 == c2 && i < n);
  return c1 - c2;
}

void* memset(void* v,int c,size_t n) {
  unsigned char val = (unsigned char)c;
  for (int i = 0; i < n; i++) {
    ((unsigned char*)v)[i] = val;
  }
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  char *d = dst;
  const char *s = src;
  if (d < s)
    while (n--)
      *d++ = *s++;
  else
    {
      const char *lasts = s + (n-1);
      char *lastd = d + (n-1);
      while (n--)
        *lastd-- = *lasts--;
    }
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  for (int i = 0; i < n; i++) {
    ((unsigned char*)out)[i] = ((unsigned char*)in)[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  const char *c1 = s1, *c2 = s2;
  for (int i = 0; i < n; i++) {
    if (c1[i] != c2[i]) return c1[i] - c2[i];
  }
  return 0;
}

#endif

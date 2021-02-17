#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(0);
  return 0;
}

char *strcpy(char* dst,const char* src) {
  assert(0);
  return NULL;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert(0);
  return NULL;
}

char* strcat(char* dst, const char* src) {
  assert(0);
  return NULL;
}

int strcmp(const char* s1, const char* s2) {
  unsigned char c1, c2;
  do {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
  }
  while (c1 == c2);
  return c1 - c2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(0);
  return 0;
}

void* memset(void* v,int c,size_t n) {
  assert(0);
  return NULL;
}

void* memmove(void* dst,const void* src,size_t n) {
  assert(0);
  return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(0);
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  const char *c1 = s1, *c2 = s2;
  for (int i = 0; i < n; i++) {
    if (c1[i] != c2[i]) return c1[i] - c2[i];
  }
  return 0;
}

#endif

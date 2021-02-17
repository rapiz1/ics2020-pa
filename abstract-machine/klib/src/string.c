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
  for (;*s1 && *s2; s1++, s2++) {
    if (*s1 != *s2)
      return *s1 - *s2;
  }
  return *s1 == 0 && *s2 == 0;
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

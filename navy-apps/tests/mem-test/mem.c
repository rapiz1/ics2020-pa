#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

#define PGSIZE (1<<12)

int main() {
	int sz = PGSIZE*11 + 8;
	int *mem = malloc(sz);
	int n = sz/sizeof(int);
	for (int i = 0; i < n; i++) mem[i] = i;
	for (int i = 0; i < n; i++) assert(mem[i] == i);
	return 0;
}

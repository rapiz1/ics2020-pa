#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int fib(int n) {
	if (n == 0 || n == 1) return 1;
	return fib(n-1) + fib(n-2);
}
int main() {
	for (int i = 0; i < 32; i++) printf("%d:%d\n", i, fib(i));
	return 0;
}

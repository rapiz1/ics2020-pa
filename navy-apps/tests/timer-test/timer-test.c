#include <stdio.h>
#include <sys/time.h>

int main() {
  int sec = 1;
	struct timeval tv;
  while (1) {
		while (1) {
			gettimeofday(&tv, NULL);
			if (tv.tv_sec > sec)
				break;
		}
    if (sec == 1) {
      printf("%d second\n", sec);
    } else {
      printf("%d seconds\n", sec);
    }
    sec ++;
  }
  return 0;
}

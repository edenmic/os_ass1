#include "kernel/types.h"
#include "user/user.h"

int main(void) {
  printf("Initial memory size: %d bytes\n", memsize()); //2.a

  char *ptr = malloc(20 * 1024);  // 2.b. Allocate 20 KB
  if (ptr == 0) {
    printf("Memory allocation failed\n");
    exit(1, "");
  }

  printf("After malloc: %d bytes\n", memsize()); //2.c

  free(ptr); //2.c

  printf("After free: %d bytes\n", memsize()); //2.d
  exit(0, "");
  return 0; 
}

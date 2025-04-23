#include "kernel/types.h"
#include "user/user.h"

int main(void) {
  printf("Initial memory size: %d bytes\n", memsize());

  char *ptr = malloc(20 * 1024);  // Allocate 20 KB
  if (ptr == 0) {
    printf("Memory allocation failed\n");
    exit(1, "");
  }

  printf("After malloc: %d bytes\n", memsize());

  free(ptr);

  printf("After free: %d bytes\n", memsize());

  exit(0, "");
  return 0; 
}

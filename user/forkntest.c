#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int n = 4;
  int pids[16];  // Enough space for up to 16 children

  int ret = forkn(n, pids);  // Call your new forkn syscall
  
  if (ret < 0) {
    printf("forkntest: forkn failed\n");
    exit(1, "");
  }

  if (ret == 0) {
    // Parent process
    printf("forkntest: parent created %d children\n", n);
    printf("Child PIDs:\n");
    for (int i = 0; i < n; i++) {
      printf("  Child %d PID: %d\n", i + 1, pids[i]);
    }

    // Wait for children (for now use wait, later will replace with waitall)
    for (int i = 0; i < n; i++) {
      wait(0, 0);
    }

    printf("forkntest: all children finished\n");
  } else {
    // Child process: ret is the child number (1..n)
    printf("forkntest: I am child number %d, my PID is %d\n", ret, getpid());
    exit(0, "");  // Children exit immediately
  }

  exit(0, "");
  return 0;  // To satisfy compiler (even though exit handles it)
}

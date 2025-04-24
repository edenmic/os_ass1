#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 4  // Number of child processes to create

int
main(void)
{
    int n;
    int statuses[NPROC];

    // Create child processes
    for (int i = 0; i < NPROC; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            exit(1, "");
        }
        if (pid == 0) {
            // Child process
            printf("Child %d: exiting with status %d\n", i + 1, i + 10);
            exit(i + 10, "");  // Exit with a unique status
        }
    }

    // Wait for all child processes to finish
    if (waitall(&n, statuses) == 0) {
        printf("Number of exited children: %d\n", n);
        for (int i = 0; i < n; i++) {
            printf("Child %d exited with status %d\n", i + 1, statuses[i]);
        }
    } else {
        printf("waitall failed\n");
    }

    exit(0, "");
    return 0;  // To satisfy compiler (even though exit handles it)
}
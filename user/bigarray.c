#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE 65536  // 2^16
#define NPROC 4           // Number of child processes

int
main(void)
{
    int *array = malloc(ARRAY_SIZE * sizeof(int));  // Allocate array on the heap
    if (array == 0) {
        printf("Failed to allocate memory for array\n");
        exit(1, "");
    }

    int pids[NPROC];
    int n = NPROC;
    int statuses[NPROC];
    uint sum_of_sums = 0;  // Use unsigned int to avoid overflow

    // Initialize the array with values 0 to ARRAY_SIZE - 1
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i;
    }

    // Create child processes using forkn
    if (forkn(n, pids) < 0) {
        printf("forkn failed\n");
        free(array);  // Free allocated memory before exiting
        exit(1, "");
    }

    // Child processes
    for (int i = 0; i < n; i++) {
        if (pids[i] == 0) {  // Child process
            int start = (ARRAY_SIZE / n) * i;
            int end = (ARRAY_SIZE / n) * (i + 1);
            uint sum = 0;  // Use unsigned int to avoid overflow

            // Calculate the sum of the assigned quarter
            for (int j = start; j < end; j++) {
                sum += array[j];
            }

            printf("Child %d (PID %d): sum = %d\n", i + 1, getpid(), sum);  // Use %d
            free(array);  // Free allocated memory in the child process
            exit(sum, "");  // Exit with the calculated sum
        }
    }

    // Parent process
    printf("Parent: Created %d child processes\n", n);
    for (int i = 0; i < n; i++) {
        printf("Child PID: %d\n", pids[i]);
    }

    // Wait for all child processes to finish using waitall
printf("before waitall, n = %d\n", n);
printf("before waitall, n = %d\n", n);
    if (waitall(&n, statuses) < 0) {
        printf("waitall failed\n");
        free(array);  // Free allocated memory before exiting
        exit(1, "");
    }

    printf("n after waitall = %d\n", n);  // Check if n is correct!

    // Calculate the sum of the sums
    for (int i = 0; i < n; i++) {
        printf("DEBUG: statuses[%d] = %d\n", i, statuses[i]);

        uint sum = (uint)statuses[i];  // Cast to unsigned int   
        printf("DEBUG: sum = %d\n", sum);  // Use %d
        printf("Child %d returned sum = %d\n", i + 1, sum);  // Use %d
        sum_of_sums += sum;
    }

    printf("Parent: Sum of sums = %d\n", sum_of_sums);  // Use %d

    free(array);  // Free allocated memory in the parent process
    exit(0, "");
    return 0;  // To satisfy compiler (even though exit handles it)
}
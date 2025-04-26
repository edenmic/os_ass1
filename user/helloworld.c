// #include "kernel/types.h"
// #include "user/user.h"

// int main(void) {
//   printf("Hello World xv6\n");
//   exit(0, "");
//   return 0; // never reached  
// }
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    // No arguments provided, print default message
    printf("Hello World xv6\n");
  } else {
    printf("Hello World doesn't need any other arguments\n");
  }
  exit(0, "");
  return 0;  // This line is not necessary but included for clarity.
}
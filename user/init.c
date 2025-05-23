// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
       exit(0, "init: fork failed");
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
       exit(0, "init: fork failed");
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      int dummy;
      char msg[32];
      wpid = wait(&dummy, msg);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
         exit(0, "init: fork failed");
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}

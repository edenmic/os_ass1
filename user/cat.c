#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(0, "cat: error 1");
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(0, "cat: error 1");
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit(0, "cat: error 0");
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(0, "cat: error 1");
    }
    cat(fd);
    close(fd);
  }
  exit(0, "cat: error 0");
  return 0;
}

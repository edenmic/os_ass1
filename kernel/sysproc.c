#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
// sys_exit(void)
// {
//   char msg[32];
//   if (argstr(0, msg, 32) < 0) //32 is int max
//     return -1;
//   exit(msg);
//   return 0;  // not reached
// }
sys_exit(void)
{
  int status;
  char msg[32];

  argint(0, &status);

  if (argstr(1, msg, 32) < 0)
    return -1;

  struct proc *p = myproc();
  safestrcpy(p->exit_msg, msg, 32);  // This must happen before calling exit()

  exit(status, msg);  // This will mark the process as ZOMBIE
  return 0;      // Never reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  //uint64 p;
  //argaddr(0, &p);
  //return wait(p);
  uint64 addr;     // address to pass xstate
  uint64 msgaddr;  // address to pass exit message

  //if (argaddr(0, &addr) < 0 || argaddr(1, &msgaddr) < 0)
  //  return -1;
  //return wait(addr, msgaddr);
  argaddr(0, &addr);
  argaddr(1, &msgaddr);

  return wait(addr, msgaddr);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Explanation:
//myproc() gets the current process.
//.sz is the size of the process memory in bytes.//
uint64
sys_memsize(void)
{
  return myproc()->sz;
}

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

// uint64
// sys_forkn(int n)
// {
//   uint64 pids_addr;

//   argint(0, &n);                      // First argument: number of children
//   argaddr(1, &pids_addr);             // Second argument: pointer to PID array

//   if (n < 1 || n > 16)
//     return -1;

//   struct proc *p = myproc();
//   struct proc *children[16];          // Store pointers to created children
//   int pids[16];
//   int created = 0;

//   // First phase: try to allocate all children
//   for (int i = 0; i < n; i++) {
//     struct proc *np = allocproc();
//     if (np == 0) {
//       // Cleanup already allocated children if any failure
//       for (int j = 0; j < created; j++) {
//         acquire(&children[j]->lock);
//         children[j]->killed = 1;
//         children[j]->state = ZOMBIE;
//         release(&children[j]->lock);
//       }
//       return -1;
//     }

//     // Copy parent's state into child
//     np->parent = p;
//     np->trapframe = kalloc();
//     if (np->trapframe == 0) {
//       freeproc(np);
//       return -1;
//     }
//     *(np->trapframe) = *(p->trapframe);

//     np->pagetable = proc_pagetable(np);
//     if (np->pagetable == 0) {
//       freeproc(np);
//       return -1;
//     }

//     safestrcpy(np->name, p->name, sizeof(p->name));

//     // Store the child pointer and PID for later
//     children[created] = np;
//     pids[created] = np->pid;
//     created++;
//   }

//   // Copy child PIDs to userspace (in the parent process)
//   if (copyout(p->pagetable, pids_addr, (char *)pids, n * sizeof(int)) < 0)
//     return -1;

//   // ✅ Phase 2: Release all children (after all succeeded)
//   for (int i = 0; i < created; i++) {
//     if (children[i] != myproc()) {                  // ✅ Add this check!
//       acquire(&children[i]->lock);
//       children[i]->childnum = i + 1;
//       children[i]->state = RUNNABLE;
//       release(&children[i]->lock);
//     }
//   }

//   return 0;  // Parent returns 0
// }



uint64
sys_waitall(void)
{
  // Not implemented yet — just return -1 for now!
  return -1;
}


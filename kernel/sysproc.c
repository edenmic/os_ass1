#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#ifndef NULL
#define NULL ((void *)0)
#endif
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
sys_forkn(void)
{
  printf("starting forkn()\n");
  int n;
  uint64 pids_addr;  // user-space pointer to array of pids

  argint(0, &n);
  argaddr(1, &pids_addr);

  if(n < 1 || n > 16)
    return -1;
  printf("line 213\n");

  struct proc *p = myproc();
  struct proc *children[16];  // array of child process pointers
  int created = 0;
  printf("line 218\n");

  // Allocate n children
  for(int i = 0; i < n; i++){
    printf("im in the for loop, i = %d \n", i);
    struct proc *np;
    printf("line 224\n");
    if ((np = allocproc()) == 0) {
        printf("allocproc failed at i = %d\n", i);
        // Clean up already created children
        for (int j = 0; j < created; j++) {
            if (children[j] != NULL) {
                printf("Cleaning up child %d\n", children[j]->pid);
                printf("Before acquiring lock for child %d\n", j);
                acquire(&children[j]->lock);
                printf("Acquired lock for child %d\n", j);
                children[j]->state = UNUSED;
                release(&children[j]->lock);
            }
        }
        return -1;
    }

    // Copy parent user memory to child
    if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
      freeproc(np);
      release(&np->lock);
      for (int j = 0; j < created; j++) {
        acquire(&children[j]->lock);
        children[j]->state = UNUSED;
        release(&children[j]->lock);
      }
      return -1;
    }

    np->sz = p->sz;
    *(np->trapframe) = *(p->trapframe);
    np->trapframe->a0 = i + 1;  // child's return value is its index (1-based)

    for(int fd = 0; fd < NOFILE; fd++){
      if(p->ofile[fd])
        np->ofile[fd] = filedup(p->ofile[fd]);
    }
    np->cwd = idup(p->cwd);
    safestrcpy(np->name, p->name, sizeof(p->name));
    //pid = np->pid;

    release(&np->lock); //


    acquire(&wait_lock);
    np->parent = p;
    release(&wait_lock);

    children[created++] = np;  // save the child in the array
  }

  // All children created successfully, set them RUNNABLE
  for(int i = 0; i < created; i++){
    acquire(&children[i]->lock);
    children[i]->state = RUNNABLE;
    release(&children[i]->lock);
  }

  // Prepare PIDs array to copy back to userspace
  int pid_array[16];
  for(int i = 0; i < n; i++){
    pid_array[i] = children[i]->pid;
  }

  // Copy PIDs to user space
  if(copyout(p->pagetable, pids_addr, (char *)pid_array, n * sizeof(int)) < 0){
    // If copyout fails, we still already made the children RUNNABLE. Can't clean them.
    return -1;
  }

  return 0;  // parent returns 0
}


uint64
sys_waitall(void)
{
    uint64 n_addr, statuses_addr;
    argaddr(0, &n_addr);
    argaddr(1, &statuses_addr);

    struct proc *p = myproc();
    int statuses[NPROC];  // Array to store exit statuses of child processes
    int count = 0;        // Number of child processes that have exited

    acquire(&wait_lock);

    for (;;) {
        int havekids = 0;
        int found = 0;

        // Iterate over all processes in the process table
        for (struct proc *pp = proc; pp < &proc[NPROC]; pp++) {
          //printf("Checking process pp = %d\n", pp);
          //printf("Process %d, parent: %d, state: %d\n", pp->pid, pp->parent ? pp->parent->pid : -1, pp->state);
            if (pp->parent == p) {
                acquire(&pp->lock);
                havekids = 1;

                if (pp->state == ZOMBIE) {
                    // Found a child process in the ZOMBIE state
                    printf("Found ZOMBIE process %d\n", pp->pid);
                    statuses[count++] = pp->xstate;  // Collect its exit status
                    freeproc(pp);                   // Free the process
                    release(&pp->lock);
                    found = 1;
                } else {
                    release(&pp->lock);
                }
            }
        }

        if (!havekids) {
            // No children exist
            printf("DEBUG: No children exist for process %d\n", p->pid);
            release(&wait_lock);

            // Copy the number of exited children to user space
            if (copyout(p->pagetable, n_addr, (char *)&count, sizeof(int)) < 0) {
                printf("DEBUG: Failed to copy count to user space\n");
                return -1;
            }

            // Copy the statuses array to user space
            if (copyout(p->pagetable, statuses_addr, (char *)statuses, count * sizeof(int)) < 0) {
                printf("DEBUG: Failed to copy statuses to user space\n");
                return -1;
            }

            return 0;
        }

        if (!found) {
            // No ZOMBIE children found, wait for a child to exit
            printf("DEBUG: No ZOMBIE children found, sleeping process %d\n", p->pid);
            sleep(p, &wait_lock);
        }
    }

    release(&wait_lock);

    // Copy the number of exited children to user space
    if (copyout(p->pagetable, n_addr, (char *)&count, sizeof(int)) < 0) {
        printf("DEBUG: Failed to copy count to user space\n");
        return -1;
    }

    // Copy the statuses array to user space
    if (copyout(p->pagetable, statuses_addr, (char *)statuses, count * sizeof(int)) < 0) {
        printf("DEBUG: Failed to copy statuses to user space\n");
        return -1;
    }

    return 0;
}


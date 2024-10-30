#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "stat.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

addr_t
sys_sbrk(void)
{
  addr_t addr;
  addr_t n;

  argaddr(0, &n);
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define MMAP_FAILED ((~0lu))
  addr_t
sys_mmap(void)
{
  int fd, flags;
  addr_t allocation, a, new_top;
  void *mem;
  struct file *file;
  struct stat s;

  if(argint(0,&fd) < 0 || argint(1,&flags) < 0)
    return MMAP_FAILED;

  // get fd
  if (fd < 0 || fd > 9)
    return MMAP_FAILED; // bad fd

  file = proc->ofile[fd];
  if (filestat(file, &s) < 0) // get size
    return MMAP_FAILED; // bad fd

  allocation = (addr_t)proc->mmaptop;
  new_top = PGROUNDUP(allocation + s.size);

  // set metadata
  proc->mmaps[proc->mmapcount].fd = fd;
  proc->mmaps[proc->mmapcount++].start = allocation;
  proc->mmaptop = (void *)new_top;

  if (flags)
    return allocation; // lazy allocation

  // eagerly allocate pages
  for (a = allocation; a < new_top; a += PGSIZE) {
    mem = kalloc();
    if (!mem) {
      // failed to alloc
      // TODO handle properly
      /*proc->mmaptop = (void *)old_top;*/
      /*--proc->mmapcount;*/
      /*return MMAP_FAILED;*/
      panic("failed to kalloc for mmap");
    }
    memset(mem, 0, PGSIZE);
    if(mappages(proc->pgdir, (void *)a, PGSIZE, V2P(mem), PTE_W | PTE_U) < 0) {
      // ran out of memory
      // TODO fix memleaks from prior allocations
      /*kfree(mem);*/
      /*return MMAP_FAILED;*/
      panic("failed to map page");
    }
  }

  // flush tlb
  /*lcr3(V2P(proc->pgdir));*/

  // read file
  fileread(file, uva2ka(proc->pgdir, (void *)allocation), s.size);

  return allocation;
}

  int
handle_pagefault(addr_t va)
{
  // TODO: your code here
  return 0;
}

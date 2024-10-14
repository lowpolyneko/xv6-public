#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "vga.h"

#define DISPLAY 2 // init.c creates the display as file 2
#define CRTPORT 0x3d4
#define CRT_BUF_SIZE sizeof(ushort)*24*80

static char *vga = (char*)P2V(0xA0000);  // VGA memory

static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
static ushort crt_buf[24*80]; // buffer for crt data
static int crt_pos;

int
displaywrite(struct file *f, char *buf, int n)
{
  int i;

  for(i = 0; i < n; i++)
    vga[f->dev_payload++] = buf[i];

  return n;
}

int
displayioctl(struct file *f, int param, int value)
{
  switch (param) {
    case 1: // switch to vga mode
      switch (value) {
        case 0x13:
          memmove(crt_buf, crt, CRT_BUF_SIZE); // save cga buffer
          outb(CRTPORT, 14); // save cursor pos
          crt_pos = inb(CRTPORT+1) << 8;
          outb(CRTPORT, 15);
          crt_pos |= inb(CRTPORT+1);
          vgaMode13();
          f->dev_payload = 0; // zero save_ptr
          return 0;
        case 0x3:
          memset(vga, 0, 65536); // clear vga buffer
          vgaMode3();
          f->dev_payload = 0x0700; // default global color
          memmove(crt, crt_buf, CRT_BUF_SIZE); // set cga buffer back
          outb(CRTPORT, 14); // set cursor pos back
          outb(CRTPORT+1, crt_pos>>8);
          outb(CRTPORT, 15);
          outb(CRTPORT+1, crt_pos);
          return 0;
        default:
          return -1;
      }
    case 2: // set vga pallete color
      // the value is a 32-bit struct containing (palette#, R, G, B)
      vgaSetPalette(value >> 24 & 0xFF, value >> 16 & 0xFF, value >> 8 & 0xFF, value & 0xFF);
      return 0;
  }
  cprintf("Got unknown display ioctl request. %d = %d\n",param,value);
  return -1;
}

void
displayinit(void)
{
  devsw[DISPLAY].write = displaywrite;
  devsw[DISPLAY].ioctl = displayioctl;
}

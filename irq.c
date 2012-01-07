#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "sched.h"
#include "kexec.h"
#include "sys/ko.h"

#define INPUTBUFSIZE 500
volatile bool newchar;
char input[INPUTBUFSIZE];
size_t inputpos;


void kirq (PCB* stacked_pcb) __attribute__((interrupt ("IRQ")));
void kirq (PCB* stacked_pcb) {
  printk("lol");
  //rkputc('!');
}

void null_ptr_func() {
  panic("NULL PTR FUNC HIT.");
}

int update_input() {
  int c = kgetc();
  switch(c) {
  case '\r':
    c = '\n';
    kputc(c);
    return c;
    break;
  case '\b':
    /*
        kputc('\b');
        kputc(' ');
        kputc('\b');
    */
    return '\b';
  case 27:
    c = kgetc();
    if (c == 91) {
      switch (kgetc()) {
      case 'D':
        return ARROW_LEFT;
      case 'C':
        return ARROW_RIGHT;
      case 'A':
        return ARROW_UP;
      case 'B':
        return ARROW_DOWN;
      default :
        return BAD_CODE;
      }
    } else {
      return BAD_CODE;
    }
  default:
    break;
  }

  kputc(c);

  /*if (!isalpha(c) && !isdigit(c)) {
      printk("<%d>", c);
  }*/

  return c;

}


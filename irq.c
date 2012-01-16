#include "stdlib.h"
#include "syscalls.h"
#include "kio.h"
#include "user.h"
#include "mach.h"
#include "mem.h"
#include "kexec.h"
#include "sys/ko.h"
#include "task.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define INPUTBUFSIZE 4096
static char input[INPUTBUFSIZE];
static size_t max_pos = 0;
static size_t pos = 0;

static void clear() {
  memset(input, 0, INPUTBUFSIZE);
  max_pos = pos = 0;
}


static void erase_chars(size_t num) {
    for(; num > 0; num--) {
        kputs("\b \b");
    }
}

sinkhole* raw_stdin(void);

static void dispatch() {
  SINK(raw_stdin(), mk_msg(input));
  clear();
}

static volatile bool happened;
bool interrupt_happened() {
  if (happened) {
    happened = false;
    return true;
  }
  return false;
}

void idle_task() {
  enable_interrupt();
  for(;;) {
    if (interrupt_happened()) {
      disable_interrupt();
      sched();
      enable_interrupt();
    }
  }
}

void setup_irq() {
  clear();
  happened = false;
}

static int cook() {
  int c = kgetc();
  if (c == '\r') return '\n';
  if (c == 27) {
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
      kputc('?');
      return 0;
    }
  } else {
    return c;
  }
}

static void handle() 
{
    int c = cook();
    if (isspecial(c)) {
      return;
      switch (c) {
        case ARROW_LEFT:
          pos--;
          printk("max %d pos %d", max_pos, pos);
          kputc(ARROW_LEFT);
          break;
        case ARROW_RIGHT:
          pos = min(pos+1, max_pos+1);
          printk("max %d pos %d", max_pos, pos);
          kputc(ARROW_RIGHT);
          break;
        default:
          kputc('!');
          break;
      }
      return;
    }
    switch(c) { 
        case '\n':
            input[pos] = '\0';
            kputc('\n');
            dispatch();
            return;
        case '\b':
            if (pos == 0) return;
            if (pos == max_pos) {
              input[pos] = '\0';
              max_pos--;
              pos--;
              kputs("\b \b");
              return;
            } else {
              size_t len = strlen(input+pos);
              erase_chars(len);
              memmove(input+pos-1, input+pos, len);
              pos--;
              max_pos--;
              kputs(input+pos);
              return;
            }

        default:
            if (pos == max_pos) {
              input[pos] = (char) c;
              pos++;
              max_pos++;
              kputc(c);
              return;
            } else {
          printk("max %d pos %d", max_pos, pos);
              size_t len = strlen(input+pos);
              erase_chars(len);
              memmove(input+pos+1, input+pos, len);
              input[pos] = c;
              kputs(input+pos);
              pos++;
              max_pos++;
              return;
            }
    }
}

void null_ptr_func() {
  panic("NULL PTR FUNC HIT.");
}


void kirq (void) __attribute__((interrupt ("IRQ")));
void kirq (){
  // fiddle with malloc
  vm_data* d = get_vm_base();
  reset_kernel_vm();
//  kputc('!');
  handle();
  // restore original data
  set_vm_base(d);
  happened = true;
}

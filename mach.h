#ifndef MACH_H
#define MACH_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "types.h"
#include "kio.h"
#include "stdlib.h"

/*
 *  Default (physical) base address and length:
 */

#define DEV_MP_ADDRESS          0x0000000011000000ULL
#define DEV_MP_LENGTH           0x0000000000000100ULL


/*
 *  Offsets from the base address to reach the MP device' registers:
 */

#define     DEV_MP_WHOAMI           0x0000
#define     DEV_MP_NCPUS            0x0010
#define     DEV_MP_STARTUPCPU           0x0020
#define     DEV_MP_STARTUPADDR          0x0030
#define     DEV_MP_PAUSE_ADDR           0x0040
#define     DEV_MP_PAUSE_CPU            0x0050
#define     DEV_MP_UNPAUSE_CPU          0x0060
#define     DEV_MP_STARTUPSTACK         0x0070
#define     DEV_MP_HARDWARE_RANDOM      0x0080
#define     DEV_MP_MEMORY           0x0090
#define     DEV_MP_IPI_ONE          0x00a0
#define     DEV_MP_IPI_MANY         0x00b0
#define     DEV_MP_IPI_READ         0x00c0
#define     DEV_MP_NCYCLES          0x00d0

static inline unsigned get_mem_size(void) {
    return (unsigned) ((volatile char*)DEV_MP_ADDRESS + DEV_MP_MEMORY);
}

extern void* _kstart;
extern void* _kend;

#define DEV_RTC_ADDRESS     0x0000000015000000
#define DEV_RTC_LENGTH      0x0000000000000200

#define DEV_RTC_TRIGGER_READ    0x0000
#define DEV_RTC_SEC    0x0010
#define DEV_RTC_USEC    0x0020

#define DEV_RTC_HZ    0x0100
#define DEV_RTC_INTERRUPT_ACK    0x0110


#define DEV_IRQC_ADDRESS    0x0000000016000000
#define DEV_IRQC_LENGTH     0x12

#define DEV_IRQC_IRQ        0x0
#define DEV_IRQC_MASK       0x4
#define DEV_IRQC_UNMASK     0x8


#define IRQ_MP 6
#define IRQ_RTC 4
#define IRQ_ETHER 3
#define IRQ_CONS 2

#define USER_STACK_SIZE 10

static inline void enable_rtc_irq(void)
{
	*(volatile unsigned *) (DEV_IRQC_ADDRESS + DEV_IRQC_UNMASK) = IRQ_RTC;
}

static inline void set_timer(unsigned usec) {
  *(volatile int*)(DEV_RTC_ADDRESS + DEV_RTC_HZ) = usec;
}

static inline unsigned __get_CPSR(void) {
    unsigned temp;
    asm volatile ("mrs %0,CPSR":"=r" (temp):) ;
    return temp;
}
static inline void __set_CPSR(unsigned save_cpsr) 
{
    asm volatile (" msr CPSR_c,%0"::"r"(save_cpsr) );
}

static inline void __set_CPR1(unsigned saved) {
    asm volatile ("mcr p15, 0, %[temp], c0,c0"
        :[temp]"=r" (saved)
        );
}

static inline unsigned __get_CPR1(void) { 
    unsigned temp;
    asm volatile ("mrc p15, 0, %[temp], c0,c0"
        :[temp]"=r" (temp)
        );
    return temp;
}

static inline void __set_CPR0(unsigned saved) {
    asm volatile ("mcr p15, 0, %[temp], c1,c0"
        :[temp]"=r" (saved)
        );
}

static inline unsigned __get_CPR0(void) { 
    unsigned temp;
    asm volatile ("mrc p15, 0, %[temp], c1,c0"
        :[temp]"=r" (temp)
        );
    return temp;
}


static inline void enable_interrupt(void){
    unsigned temp;
    temp = __get_CPSR();
        __set_CPSR(temp & ~128);
}

/* Disable interrupts and save CPSR */
static inline unsigned disable_interrupt(void){
    unsigned temp;
    temp = __get_CPSR();
    __set_CPSR(temp | 128);
    return temp;
}
/*
static inline unsigned disable_irq(void){
    unsigned temp;
    temp = __get_CPSR();
    __set_CPSR(temp | 0x80);
    return temp;
}*/

typedef unsigned klock_t;
#define KLOCK_INIT 0

static inline void klock(klock_t* l) {
  if (*l & 128) {
 //   printk("l %x", *l & 128);
  }
  *l = disable_interrupt();
}

static inline void kunlock(klock_t* l) {
  __set_CPSR(*l);
  *l = 0;
}
  


void enable_irq(int clobber);
void disable_irq(int clobber);

static inline unsigned change_mode(unsigned mode)
{
    printk("Change mode to %x\n", mode);
    unsigned temp = __get_CPSR();
    unsigned temp2 = temp & (~0x1f);
    __set_CPSR(temp2 | mode);
    return temp;
}
void switch_to_user(void* stack, void* retptr, void *start, int d3);

void set_irq_stack(void* stack, int dummy, int dummy2, int dummy3);

static inline void enable_mmu(void)
{
    unsigned temp = __get_CPR0();
    __set_CPR1(temp | 0x1);
}

static inline void get_cpuid(void)
{
    unsigned temp = __get_CPR1();
    printk("CPUID: %u\n", temp);
}


/* Restore interrupt status */
static inline void restore_interrupt(unsigned saved_CPSR){
    __set_CPSR (saved_CPSR);
}


static inline void enable_cons_irq(void)
{
  printk("%p", DEV_IRQC_ADDRESS + DEV_IRQC_MASK);
	*(volatile unsigned *)(DEV_IRQC_ADDRESS + DEV_IRQC_UNMASK) = IRQ_CONS;
}

static inline void disable_cons_irq(void)
{
	*(volatile unsigned *)(DEV_IRQC_ADDRESS + DEV_IRQC_MASK) = IRQ_CONS;
}

static inline unsigned get_irq_mask()
{
	return *(volatile unsigned *)( DEV_IRQC_ADDRESS + DEV_IRQC_IRQ);
}


#endif

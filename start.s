interrupt_vector_table:
    b _start @ Reset
    b halt @ bad instr
    b swi_handler 
    b halt @ prefetch abort 
    b halt @ dab_handler @ data abort
    b halt
    b irq_handler @ irq
    b halt @fiq

.comm stack, 0x10000 @ Reserve 64k stack in the BSS
_start:
    .globl _start
    ldr sp, =stack+0x10000 @ Set up the stack
    bl kmain @ Jump to the main function
1: 
    b 1b @ Halt

.global syscall
syscall:
    str r5, [sp, #-4]
    swi 0x0
    mov r0, r5
    ldr r5, [sp, #-4]
    bx lr

.global set_swi_stack
set_swi_stack:
    mrs r1,CPSR
    @ save for later
    mov r2, r1
    mov r3, sp

    @ change mode to 0x12 (swi)
    and r1, #~0x1f
    orr r1, #0x12
    msr CPSR,r1
    @ setup stack
    mov sp, r0

    @ retore state
    msr CPSR, r2
    mov sp, r3
    bx lr

.global switch_to_user
switch_to_user:
    mrs r3,CPSR
    
    @ change mode to 0x10 (user)
    and r3, #~0x1f
    orr r3, #0x10
    msr CPSR,r3
    @ setup stack
    mov sp, r0

    mov lr, r1
    mov pc, r2






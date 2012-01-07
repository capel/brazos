.text
.global ivt
ivt:
    .global null_addr
    null_addr:
    b _start @ Reset
    b khalt @ bad instr
    b swi_handler 
    b khalt @ prefetch abort 
    b khalt @ dab_handler @ data abort
    b khalt
    b kirq @ irq
    b khalt @fiq

.comm stack, 0x10000 @ Reserve 64k stack in the BSS
.comm irq_stack, 0x10000 @ stack for IRQ

_start:
    .globl _start
    ldr r1, =null_ptr_func
    ldr r0, =0
    str r1, [r0]

    @ go into IRQ mode
    ldr r1, =0x12
    msr CPSR_c, r1

    @ setup IRQ stack
    ldr sp, =irq_stack+0x10000

    @return to system mode
    ldr r1, =0x13
    msr CPSR_c, r1
    
    @ setup swi/kernel stack
    ldr sp, =stack+0x10000 @ Set up the stack
    bl kmain @ Jump to the main function
1: 
    b 1b @ Halt

.text
.align 2
.global enable_irq
enable_irq:
  mrs r1, CPSR
  bic r1, #0x80
  msr CPSR_c, r1
  bx lr

.text
.align 2
.global syscall
syscall:
    swi 0x0
    bx lr

.text
.align 2
.global set_swi_stack
set_swi_stack:
    mrs r1,CPSR
    @ save for later
    mov r2, r1
    mov r3, sp

    @ change mode to 0x12 (swi)
    bic r1, #0x1f
    orr r1, #0x12
    msr CPSR,r1
    @ setup stack
    mov sp, r0

    @ retore state
    msr CPSR, r2
    mov sp, r3
    bx lr


.text
.align 2
.global swi_handler
swi_handler:
    @ save it now, since it will get clobbered
    @ lr is their pc + 4
    stmfd sp!, {lr}

    bl save_pcb
    bl ksyscall

    @ no return
    bl restore_pcb

.text
.align 2
.global irq_handler
irq_handler:
    @stmfd sp!, {lr}
    
    @ bl save_pcb
    
    mov r1, #0x0000000010000000
    mov r0, #0x33
    str r1, [r0]

     @bl restore_pcb

.text
.align 2
save_pcb:

    @ save pcb stuff
    stmfd sp!, {r0-r14}^
    
    mrs r0, spsr
    stmfd sp!, {r0}

    @ r0 = address of PCB on stack (the top)
    mov r0, sp
    mov pc, lr

.text
.align 2
.global restore_pcb
restore_pcb:
    @ pcb block is in r0

    @ clean up our stack space
    add sp, #68

    @ return state for the pcb
    mov lr, r0 @ lr is banked, so we are borrowing it
    
    ldmfd lr!, {r1} @hopefully spsr
    msr spsr, r1
    
    ldmfd lr!, {r0-r14}^
    ldmfd lr!, {pc}^


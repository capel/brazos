interrupt_vector_table:
    b _start @ Reset
    b khalt @ bad instr
    b swi_handler 
    b khalt @ prefetch abort 
    b khalt @ dab_handler @ data abort
    b khalt
    b irq_handler @ irq
    b khalt @fiq

.comm stack, 0x10000 @ Reserve 64k stack in the BSS

_start:
    .globl _start
    
    ldr sp, =stack+0x10000 @ Set up the stack
    bl kmain @ Jump to the main function
1: 
    b 1b @ Halt




.text
.align 2
.global syscall
syscall:
    str r5, [sp, #-4]
    swi 0x0
    ldr r5, [sp, #-4]
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
.global switch_to_user
switch_to_user:
    @ r0 is process*
    mrs r3,CPSR
    
    @ change mode to 0x10 (user)
    bic r3, #0x1f
    orr r3, #0x1
    msr CPSR,r3
    @ setup stack
    mov sp, r0

    mov lr, r1
    mov pc, r2

.text
.align 2
.global swi_handler
swi_handler:
    bl save_pcb

    bl ksyscall

    @ no return
    b restore_pcb

.text
.align 2
.global irq_handler
irq_handler:
    bl save_pcb

    bl kirq

    b restore_pcb

.text
.align 2
save_pcb:

    @ save pcb stuff
    stmfd sp!, {lr}
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
    sub sp, #68

    @ return state for the pcb
    mov lr, r0 @ lr is banked, so we are borrowing it
    
    ldmfd lr!, {r1} @hopefully spsr
    msr spsr, r1
    
    ldmfd lr!, {r0-r14}^
    ldmfd lr!, {pc}^


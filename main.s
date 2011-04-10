	.file	"main.c"
	.text
	.align	2
	.type	gets, %function
gets:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	str	r1, [fp, #-12]
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-12]
	mov	r0, #1
	mov	r1, r2
	mov	r2, r3
	mov	r3, #0
	bl	syscall
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
	.size	gets, .-gets
	.align	2
	.type	get_pages, %function
get_pages:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #4
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	mov	r0, #4
	mov	r1, r3
	mov	r2, #0
	mov	r3, #0
	bl	syscall
	mov	r3, r0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
	.size	get_pages, .-get_pages
	.align	2
	.type	halt, %function
halt:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bl	syscall
	ldmfd	sp!, {fp, pc}
	.size	halt, .-halt
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Hello\012\000"
	.align	2
.LC1:
	.ascii	"Page %p\012\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 504
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #504
	ldr	r0, .L6
	bl	printf
	mov	r0, #1
	bl	get_pages
	mov	r3, r0
	str	r3, [fp, #-8]
	ldr	r0, .L6+4
	ldr	r1, [fp, #-8]
	bl	printf
.L5:
	sub	r3, fp, #508
	mov	r0, r3
	mov	r1, #500
	bl	gets
	sub	r3, fp, #508
	mov	r0, r3
	bl	printf
	b	.L5
.L7:
	.align	2
.L6:
	.word	.LC0
	.word	.LC1
	.size	main, .-main
	.section	.rodata
	.align	2
.LC2:
	.ascii	"Death comes for us all\012\000"
	.text
	.align	2
	.global	exit
	.type	exit, %function
exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r0, .L9
	bl	printf
	bl	halt
	ldmfd	sp!, {fp, pc}
.L10:
	.align	2
.L9:
	.word	.LC2
	.size	exit, .-exit
	.align	2
	.type	__get_CPSR, %function
__get_CPSR:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	stmfd	sp!, {r4, fp}
	add	fp, sp, #4
	sub	sp, sp, #4
@ 81 "mach.h" 1
	mrs r4,CPSR
@ 0 "" 2
	str	r4, [fp, #-8]
	ldr	r3, [fp, #-8]
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {r4, fp}
	bx	lr
	.size	__get_CPSR, .-__get_CPSR
	.align	2
	.type	__set_CPSR, %function
__set_CPSR:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #4
	str	r0, [fp, #-4]
	ldr	r3, [fp, #-4]
@ 85 "mach.h" 1
	 msr CPSR_c,r3
@ 0 "" 2
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	__set_CPSR, .-__set_CPSR
	.align	2
	.type	__get_CPR1, %function
__get_CPR1:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	stmfd	sp!, {r4, fp}
	add	fp, sp, #4
	sub	sp, sp, #4
@ 96 "mach.h" 1
	mrc p15, 0, r4, c0,c0
@ 0 "" 2
	str	r4, [fp, #-8]
	ldr	r3, [fp, #-8]
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {r4, fp}
	bx	lr
	.size	__get_CPR1, .-__get_CPR1
	.align	2
	.type	enable_interrupt, %function
enable_interrupt:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #4
	bl	__get_CPSR
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	bic	r3, r3, #128
	mov	r0, r3
	bl	__set_CPSR
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
	.size	enable_interrupt, .-enable_interrupt
	.section	.rodata
	.align	2
.LC3:
	.ascii	"CPUID: %u\012\000"
	.text
	.align	2
	.type	get_cpuid, %function
get_cpuid:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #4
	bl	__get_CPR1
	str	r0, [fp, #-8]
	ldr	r0, .L16
	ldr	r1, [fp, #-8]
	bl	printk
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L17:
	.align	2
.L16:
	.word	.LC3
	.size	get_cpuid, .-get_cpuid
	.align	2
	.type	enable_cons_irq, %function
enable_cons_irq:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	mov	r3, #369098752
	add	r3, r3, #8
	mov	r2, #2
	str	r2, [r3, #0]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	enable_cons_irq, .-enable_cons_irq
	.comm	newchar,4,4
	.comm	input,500,4
	.comm	inputpos,4,4
	.comm	pcb,4,4
	.align	2
	.global	irq_handler
	.type	irq_handler, %function
irq_handler:
	@ Interrupt Service Routine.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	stmfd	sp!, {r2, r3, fp}
	add	fp, sp, #8
	ldr	r3, .L20
	mov	r2, #1
	str	r2, [r3, #0]
	sub	sp, fp, #8
	ldmfd	sp!, {r2, r3, fp}
	subs	pc, lr, #4
.L21:
	.align	2
.L20:
	.word	newchar
	.size	irq_handler, .-irq_handler
	.align	2
	.global	dab_handler
	.type	dab_handler, %function
dab_handler:
	@ Interrupt Service Routine.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	sub	lr, lr, #4
	stmfd	sp!, {r0, r1, r2, r3, fp, ip, lr}
	add	fp, sp, #24
	bl	halt
	sub	sp, fp, #24
	ldmfd	sp!, {r0, r1, r2, r3, fp, ip, pc}^
	.size	dab_handler, .-dab_handler
	.align	2
	.global	update_input
	.type	update_input, %function
update_input:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #4
	mov	r3, #0
	strb	r3, [fp, #-5]
.L27:
	bl	kgetch
	mov	r3, r0
	strb	r3, [fp, #-6]
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L24
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L24:
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	cmp	r3, #13
	bne	.L25
	mov	r3, #10
	strb	r3, [fp, #-6]
.L25:
	ldr	r3, .L28
	ldr	r2, [r3, #0]
	mov	r3, #496
	add	r3, r3, #1
	cmp	r2, r3
	bhi	.L26
	ldr	r3, .L28
	ldr	r3, [r3, #0]
	ldr	r2, .L28+4
	ldrb	r1, [fp, #-6]
	strb	r1, [r2, r3]
	add	r2, r3, #1
	ldr	r3, .L28
	str	r2, [r3, #0]
	ldr	r3, .L28
	ldr	r3, [r3, #0]
	ldr	r2, .L28+4
	mov	r1, #0
	strb	r1, [r2, r3]
.L26:
	ldrb	r3, [fp, #-6]
	strb	r3, [fp, #-5]
	b	.L27
.L29:
	.align	2
.L28:
	.word	inputpos
	.word	input
	.size	update_input, .-update_input
	.section	.rodata
	.align	2
.LC4:
	.ascii	"Starting main\012\000"
	.text
	.align	2
	.global	kmain
	.type	kmain, %function
kmain:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #12
	ldr	r0, .L31
	bl	printk
	bl	enable_interrupt
	bl	enable_cons_irq
	bl	get_cpuid
	bl	ksetup_memory
	mov	r0, #10
	bl	kget_pages
	mov	r3, r0
	str	r3, [fp, #-8]
	mov	r0, #10
	bl	kget_pages
	mov	r3, r0
	str	r3, [fp, #-12]
	bl	kget_page
	mov	r3, r0
	str	r3, [fp, #-16]
	ldr	r2, [fp, #-16]
	ldr	r3, .L31+4
	str	r2, [r3, #0]
	ldr	r0, .L31+8
	mov	r1, #0
	mov	r2, #500
	bl	memset
	ldr	r3, .L31+12
	mov	r2, #0
	str	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	add	r3, r3, #4096
	mov	r0, r3
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bl	set_swi_stack
	ldr	r3, [fp, #-12]
	add	r3, r3, #4096
	mov	r0, r3
	ldr	r1, .L31+16
	ldr	r2, .L31+20
	mov	r3, #0
	bl	switch_to_user
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L32:
	.align	2
.L31:
	.word	.LC4
	.word	pcb
	.word	input
	.word	inputpos
	.word	exit
	.word	main
	.size	kmain, .-kmain
	.align	2
	.global	syscall_handler
	.type	syscall_handler, %function
syscall_handler:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-12]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, [fp, #-12]
	mov	r0, r2
	mov	r1, r3
	mov	r2, #68
	bl	memcpy
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	ldr	r0, [r3, #4]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	ldr	r1, [r3, #8]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	ldr	r2, [r3, #12]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	ldr	r3, [r3, #16]
	bl	ksyscall
	str	r0, [fp, #-8]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	ldr	r2, [fp, #-8]
	str	r2, [r3, #4]
	ldr	r3, .L34
	ldr	r3, [r3, #0]
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L35:
	.align	2
.L34:
	.word	pcb
	.size	syscall_handler, .-syscall_handler
	.align	2
	.global	ksyscall
	.type	ksyscall, %function
ksyscall:
	@ args = 0, pretend = 0, frame = 20
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #20
	str	r0, [fp, #-12]
	str	r1, [fp, #-16]
	str	r2, [fp, #-20]
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-12]
	sub	r3, r3, #1
	cmp	r3, #4
	ldrls	pc, [pc, r3, asl #2]
	b	.L37
.L43:
	.word	.L38
	.word	.L39
	.word	.L40
	.word	.L41
	.word	.L42
.L46:
	mov	r0, r0	@ nop
.L38:
	bl	update_input
	mov	r3, r0
	strb	r3, [fp, #-5]
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L46
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	mov	r0, r2
	ldr	r1, .L47
	mov	r2, r3
	bl	strlcpy
	ldr	r3, .L47+4
	mov	r2, #0
	str	r2, [r3, #0]
	mov	r3, #0
	b	.L45
.L39:
	ldr	r3, [fp, #-16]
	mov	r0, r3
	bl	kputs
	mov	r3, #0
	b	.L45
.L40:
	bl	khalt
	mov	r3, #0
	b	.L45
.L41:
	ldr	r3, [fp, #-16]
	mov	r0, r3
	bl	kget_pages
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-12]
	b	.L45
.L42:
	ldr	r3, [fp, #-16]
	mov	r0, r3
	bl	kfree_page
	mov	r3, #0
	b	.L45
.L37:
	b	.L36
.L45:
.L36:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L48:
	.align	2
.L47:
	.word	input
	.word	inputpos
	.size	ksyscall, .-ksyscall
	.ident	"GCC: (GNU) 4.5.0"

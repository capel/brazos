TARGET  := kernel
SRCS    := nmalloc.c sys/sched.c sys/numdir.c sys/kihashmap.c sys/procfs.c sys/message.c sys/fs.c sys/dir.c sys/bind.c sys/khashmap.c rand.c  main.c kio.c stdlib.c mem.c sched.c user.c vector.c stdio.c kexec.c sys/queue_dir.c
ASRCS   := start.s
OBJS    := ${SRCS:.c=.o} 
AOBJS   := ${ASRCS:.s=.o}
DEPS    := ${SRCS:.c=.dep} 

CCFLAGS=-Wall -Wextra -nostdlib -nostartfiles -ffreestanding -std=gnu99 -Wno-unused-parameter
CC=arm-elf-gcc-4.6
AS=arm-elf-as
LDFLAGS=-T link.ld
LD=arm-elf-ld

.PHONY: all clean distclean 
all: ${TARGET} 
	./mkfs disk
	gxemul -K  -VT -d disk -E testarm kernel

${TARGET}: ${OBJS} ${AOBJS}
	${LD} ${LDFLAGS} -o ${TARGET} start.o ${OBJS}

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $< 

${AOBJS}: %.o: %.s
	${AS} -o $@ $<

clean:
	rm -f *.o ${TARGET} 
	rm -f sys/*.o

distclean:: clean

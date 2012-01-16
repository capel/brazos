TARGET  := kernel
SRCS    := malloc.c sys/sched.c sys/message.c sys/fs.c sys/dir.c sys/bind.c sys/idir.c rand.c  kio.c stdlib.c mem.c user.c vector.c stdio.c kexec.c sys/queue_dir.c sys/future.c sys/ko.c sys/walk.c parse_dir.c irq.c setup.c swi.c task.c
ASRCS   := start.s
OBJS    := ${SRCS:.c=.o} 
AOBJS   := ${ASRCS:.s=.o}
DEPS    := ${SRCS:.c=.dep} 

CCFLAGS=-Wall -Werror -Wextra -nostartfiles -nostdlib -ffreestanding -std=gnu99 -Wno-unused-parameter -Wno-unused-function
CC=arm-elf-gcc-4.6
AS=arm-elf-as
LDFLAGS=-T link.ld
LD=arm-elf-ld

.PHONY: all clean distclean 
all: ${TARGET} 
	gxemul -K -VT -d disk -E testarm kernel

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

TARGET  := kernel
COMMON_SRC := common/rand.c common/hashmap.c common/vector.c common/malloc.c common/stdlib.c
SYS_SRC := sys/dir.c sys/kfs.c sys/main.c sys/mem.c sys/sched.c \
sys/kexec.c sys/syscall.c sys/kio.c sys/ent_gen.c sys/ent.c sys/proc.c sys/page.c
USER_SRC := user/user.c user/malloc.c user/stdio.c
SRCS 		:= ${COMMON_SRC} ${SYS_SRC} ${USER_SRC}
ASRCS   := sys/start.s
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
#	./mkfs disk
	gxemul  -VT -E testarm kernel

${TARGET}: ${OBJS} ${AOBJS}
	${LD} ${LDFLAGS} -o ${TARGET} sys/start.o ${OBJS}

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $< 

${AOBJS}: %.o: %.s
	${AS} -o $@ $<

clean:
	rm -f sys/*.o user/*.o common/*.o ${TARGET} 

distclean:: clean

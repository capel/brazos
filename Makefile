TARGET  := kernel
SRCS   := $(shell find ./ -type f -name '*.c')
ASRCS   := start.s
OBJS    := ${SRCS:.c=.o} 
AOBJS   := ${ASRCS:.s=.o}
DEPS    := ${SRCS:.c=.dep} 

CCFLAGS=-Wall -Werror -Wextra -nostartfiles -nostdlib -ffreestanding -std=gnu99 -Wno-unused-parameter -Wno-unused-function -I include
CC=arm-elf-gcc-4.6
AS=arm-elf-as
LDFLAGS=-T link.ld
LD=arm-elf-ld

.PHONY: all clean distclean 
all: ${TARGET} 
	./gxemul -K -VT -d disk -E testarm kernel

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

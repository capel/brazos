TARGET  := kernel
SRCS 		:= $(wildcard common/*.cpp sys/*.cpp user/*.cpp)
ASRCS   := sys/start.s
OBJS    := ${SRCS:.cpp=.o} 
AOBJS   := ${ASRCS:.s=.o}
DEPS    := ${SRCS:.cpp=.dep} 

CCFLAGS=-Wall -Wextra -nostdlib -nostartfiles -ffreestanding -std=c++0x -Wno-unused-parameter
CC=clang++
AS=arm-elf-as
LDFLAGS=-T link.ld
LD=arm-elf-ld

.PHONY: all clean distclean 
all: ${TARGET} 
#	./mkfs disk
	gxemul  -VT -E testarm kernel

${TARGET}: ${OBJS} ${AOBJS}
	${LD} ${LDFLAGS} -o ${TARGET} sys/start.o ${OBJS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

${AOBJS}: %.o: %.s
	${AS} -o $@ $<

clean:
	rm -f sys/*.o user/*.o common/*.o ${TARGET} 

distclean:: clean

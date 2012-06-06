TARGET  := test
SRCS   := $(shell find ./ -type f -name '*.c') ../vector.c
OBJS    := ${SRCS:.c=.o} 
AOBJS   := ${ASRCS:.s=.o}
DEPS    := ${SRCS:.c=.dep} 

CCFLAGS=-Wall -Werror -Wextra  -std=gnu99 -Wno-unused-parameter -Wno-unused-function -g3 -ferror-limit=3 -fcatch-undefined-behavior -fstack-protector-all -ggdb -I ../include/brazos
CC=clang

.PHONY: all clean distclean 
all: ${TARGET} 

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${OBJS}

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:
	rm -f *.o ${TARGET} 
	rm -f sys/*.o
	rm -f user/*.o
	rm -f test
	rm -f a.out

distclean:: clean

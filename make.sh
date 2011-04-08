 #!/bin/sh

OPTIONS="-Wall -Wextra -nostdlib -nostartfiles -ffreestanding -std=gnu99 " 
CC="arm-elf-gcc-4.5"

 arm-elf-as -o start.o start.s && \
$CC $OPTIONS -c main.c -o main.o && \
$CC $OPTIONS -c kio.c -o kio.o && \
$CC $OPTIONS -c stdlib.c -o stdlib.o && \
$CC $OPTIONS -c mem.c -o mem.o && \
 arm-elf-ld -T link.ld -o kernel start.o main.o stdlib.o kio.o mem.o && \
gxemul -vvvv -VT -E testarm kernel

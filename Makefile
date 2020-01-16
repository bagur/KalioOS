# KalioOS (C) 2020 Pranav Bagur
# Generally speaking, a cross-compiler is a compiler that runs on platform A 
# (the host), but generates executables for platform B (the target). These two 
# platforms may (but do not need to) differ in CPU, operating system, and/or 
# executable format. In our case, the host platform is the current operating 
# system, and the target platform is the operating system you are about to make.
# It is important to realize that these two platforms are not the same; your 
# operating system is always going to be different from your current operating 
# system. This is why we need to build a cross-compiler first, you will most 
# certainly run into trouble otherwise. 
#
# $@ = target file
# $< = first dependency
# $^ = all dependencies


C_SOURCES = $(wildcard kernel/*.c drivers/*.c mm/*.c common/*.c test/*.c fs/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h mm/*.h common/*.h test/*.h fs/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o kernel/interrupt.o}

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g

all: kalioOS

# First rule is run by default
kalioOS: kernel.bin boot/bootloader.bin 
	cat boot/bootloader.bin kernel.bin > kalioOS

%.bin: %.asm kernel.bin 
	bash -c "./gen_size.sh"
	nasm -i/workspace/kalioOS/boot/ -f bin $< -o $@

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: kernel/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x9100 $^ --oformat binary

# Used for debugging purposes
kernel.elf: kernel/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x9100 $^ 

# Our kernel is big enough at this point that trying to read the image as
# a floppy disk (in one go) will fail
run: kalioOS
	qemu-system-i386 -hdb kalioOS

# Open the connection to qemu and load our kernel-object file with symbols
debug: kalioOS kernel.elf
	qemu-system-i386 -s -hdb kalioOS &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	rm -rf *.bin *.dis *.o kalioOS *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o mm/*.o common/*.o test/*.o fs/*.o
	rm -rf boot/kernel_size.asm

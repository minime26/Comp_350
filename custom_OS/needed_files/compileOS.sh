#!/bin/bash

#Name: Mohammed Miahjee
 
#---Creates machine code file labeled "bootload"
nasm bootload.asm		

#---input_file=/dev/zero | output_file=diskc.img | block_size/sector=512 bytes
#---copy 512-byte of zeros 1000 times to diskc.img
dd if=/dev/zero of=diskc.img bs=512 count=1000		

#---Copy bootload into the 1st sector of diskc.img
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc

#---Copy message.txt to diskc.img at sector 30
dd if=message.txt of=diskc.img bs=512 count=1 seek=30 conv=notrunc

#---Compile kernal.c -> kernel_c.o
bcc -ansi -c -o kernel_c.o kernel.c

#---Compile kernel.asm -> kernel_asm.o
as86 kernel.asm -o kernel_asm.o

#---Link kernel_c.o & kernel_asm.o -> kernel
ld86 -o kernel -d kernel_c.o kernel_asm.o

#---Copy kernel to diskc.img, block_size=512 bytes, at sector 3
dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3

#---Launch simulator
java -jar ../emumaker86/simulator.jar

#!/bin/bash
rm -rf temp
mkdir temp
rm *.img

nasm bootloader.asm -o ./temp/bootloader.bin
nasm userproginfo.asm -o ./temp/userproginfo.bin

cd userprog
nasm b.asm -o ../temp/b.bin
nasm a.asm -o ../temp/a.bin
nasm c.asm -o ../temp/c.bin
nasm d.asm -o ../temp/d.bin
cd ..

nasm -f elf32 oskernel.asm -o ./temp/oskernel.o
nasm -f elf32 myos.asm -o ./temp/myos.o
gcc -fno-pie -c -m16 -march=i386 -masm=intel -nostdlib -ffreestanding -mpreferred-stack-boundary=2 -lgcc -shared myos_c.c -o ./temp/myos_c.o
ld -m elf_i386 -N -Ttext 0x8000 --oformat binary ./temp/oskernel.o ./temp/myos.o ./temp/myos_c.o -o ./temp/kernel.bin
rm ./temp/*.o

dd if=./temp/bootloader.bin of=Condor_OS.img bs=512 count=1 2>/dev/null
dd if=./temp/userproginfo.bin of=Condor_OS.img bs=512 seek=1 count=1 2>/dev/null
dd if=./temp/kernel.bin of=Condor_OS.img bs=512 seek=2 count=16 2>/dev/null
dd if=./temp/b.bin of=Condor_OS.img bs=512 seek=18 count=2 2>/dev/null
dd if=./temp/a.bin of=Condor_OS.img bs=512 seek=20 count=2 2>/dev/null
dd if=./temp/c.bin of=Condor_OS.img bs=512 seek=22 count=2 2>/dev/null
dd if=./temp/d.bin of=Condor_OS.img bs=512 seek=24 count=2 2>/dev/null

echo "Finished."

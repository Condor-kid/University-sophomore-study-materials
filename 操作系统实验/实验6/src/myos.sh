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
nasm interrupt_caller.asm -o ../temp/interrupt_caller.bin
nasm syscall_test.asm -o ../temp/syscall_test.bin
cd ..

cd lib
nasm -f elf32 systema.asm -o ../temp/systema.o
gcc -fno-pie -c -m16 -march=i386 -masm=intel -nostdlib -ffreestanding -mpreferred-stack-boundary=2 -lgcc -shared systemc.c -o ../temp/systemc.o
cd ..

nasm -f elf32 osstarter.asm -o ./temp/osstarter.o
nasm -f elf32 liba.asm -o ./temp/liba.o
nasm -f elf32 multiprocess.asm -o ./temp/multiprocess.o
gcc -fno-pie -c -m16 -march=i386 -masm=intel -nostdlib -ffreestanding -mpreferred-stack-boundary=2 -lgcc -shared kernel.c -o ./temp/kernel.o
ld -m elf_i386 -N -Ttext 0x8000 --oformat binary ./temp/osstarter.o ./temp/liba.o ./temp/kernel.o ./temp/systema.o ./temp/systemc.o ./temp/multiprocess.o -o ./temp/kernel.bin

dd if=./temp/bootloader.bin of=Condor_OS.img bs=512 count=1 2> /dev/null
dd if=./temp/userproginfo.bin of=Condor_OS.img bs=512 seek=1 count=1 2> /dev/null
dd if=./temp/kernel.bin of=Condor_OS.img bs=512 seek=2 count=34 2> /dev/null
dd if=./temp/b.bin of=Condor_OS.img bs=512 seek=36 count=2 2> /dev/null
dd if=./temp/a.bin of=Condor_OS.img bs=512 seek=38 count=2 2> /dev/null
dd if=./temp/c.bin of=Condor_OS.img bs=512 seek=40 count=2 2> /dev/null
dd if=./temp/d.bin of=Condor_OS.img bs=512 seek=42 count=2 2> /dev/null
dd if=./temp/interrupt_caller.bin of=Condor_OS.img bs=512 seek=44 count=1 2> /dev/null
dd if=./temp/syscall_test.bin of=Condor_OS.img bs=512 seek=45 count=3 2> /dev/null


echo "Finished."
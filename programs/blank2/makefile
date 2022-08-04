all:
	nasm -f elf ./blank2.asm -o ./build/blank2.o
	i686-elf-gcc -g -T ./linker.ld -o ./blank2.bin -ffreestanding -O0 -nostdlib -fpic -g ./build/blank2.o

clean:
	rm -rf ./build/blank2.o
all:
	nasm -f elf ./blank1.asm -o ./build/blank1.o
	i686-elf-gcc -g -T ./linker.ld -o ./blank1.bin -ffreestanding -O0 -nostdlib -fpic -g ./build/blank1.o

clean:
	rm -rf ./build/blank1.o
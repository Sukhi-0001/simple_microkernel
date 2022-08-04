FILES = ./build/kernel.asm.o ./build/kernel.o ./build/io/io.asm.o ./build/idt/idt.o  ./build/idt/idt.asm.o ./build/memory/memory.o ./build/memory/pmm/pmm.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/disk/disk.o ./build/disk/streamer.o ./build/string/string.o ./build/fs/path_parser.o ./build/fs/file.o ./build/fs/fat16/fat16.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/task/tss.asm.o ./build/task/task.asm.o ./build/task/task.o ./build/task/process.o ./build/isr80h/isr80h.o ./build/isr80h/misc.o ./build/chips/pit/pit.o ./build/scheduler/scheduler.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc


all: ./bin/boot.bin ./bin/kernel.bin user_programs
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d

#Copy a file over
	sudo cp ./hello.txt /mnt/d
	sudo cp ./programs/blank/blank.bin /mnt/d
	sudo cp ./programs/blank1/blank1.bin /mnt/d
	sudo cp ./programs/blank2/blank2.bin /mnt/d
	sudo umount /mnt/d

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c  -o ./build/kernel.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o
#idt files
./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/idt/idt.c  -o ./build/idt/idt.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o
#idt files

#memory files
./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory/memory.c  -o ./build/memory/memory.o

./build/memory/pmm/pmm.o: ./src/memory/pmm/pmm.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory/pmm/pmm.c  -o ./build/memory/pmm/pmm.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c  -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c  -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c  -o ./build/memory/heap/kheap.o
#memory files end

#disk files
./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o   

#disk files end

#string files
./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o
#string file end

#fs files start
./build/fs/path_parser.o: ./src/fs/path_parser.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/fs/path_parser.c -o ./build/fs/path_parser.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o
#fs/fat16 files
./build/fs/fat16/fat16.o: ./src/fs/fat16/fat16.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/fs/fat16/fat16.c -o ./build/fs/fat16/fat16.o
#fs/fat16/files

#fs files end

#gdt files
./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

#gdt files end

#task files
./build/task/tss.asm.o: ./src/task/tss.asm
	nasm -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

./build/task/task.asm.o: ./src/task/task.asm
	nasm -f elf -g ./src/task/task.asm -o ./build/task/task.asm.o

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

#task files end

#isr80h
./build/isr80h/isr80h.o: ./src/isr80h/isr80h.c
	i686-elf-gcc $(INCLUDES)  $(FLAGS) -std=gnu99 -c ./src/isr80h/isr80h.c -o ./build/isr80h/isr80h.o


./build/isr80h/misc.o: ./src/isr80h/misc.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr80h/misc.c -o ./build/isr80h/misc.o

#isr80h end

#chips

#chips/pit
./build/chips/pit/pit.o: ./src/chips/pit/pit.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/chips/pit/pit.c -o ./build/chips/pit/pit.o

#chips/pit end
#chips end

#scheduler
./build/scheduler/scheduler.o: ./src/scheduler/scheduler.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/scheduler/scheduler.c -o ./build/scheduler/scheduler.o

#scheduler

user_programs:
	cd ./programs/blank && $(MAKE) all
	cd ..
	cd ./programs/blank1 && $(MAKE) all
	cd ..
	cd ./programs/blank2 && $(MAKE) all

user_programs_clean:
	cd ./programs/blank && $(MAKE) clean

run:
	qemu-system-x86_64 -hda ./bin/os.bin 

debug:
	qemu-system-x86_64 -hda ./bin/os.bin -s -S

debug_gdb:
	gdb
	target remote | qemu-system-x86_64 -hda ./bin/os.bin -S -s -gdb stdio

clean: user_programs_clean
	rm -rf ./build/kernel.asm.o 
	rm -rf ./build/kernelfull.o
	rm -rf ./bin/os.bin
	rm -rf $(FILES)

 
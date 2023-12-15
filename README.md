# simple_microkernel

This project aims to create a simple operating system with its own kernel and it has its own very simple bootloader to laod the kernel

## How to build

The following tools are currently required to build this project
* A GCC Cross Compiler and how to buid one can be found here https://wiki.osdev.org/GCC_Cross-Compiler
* A GCC Linker which will be in included in above compiler
* Various Unix utilities which you probably have anyway (unless you are running Windows). Try to build and you will find out what you are missing.
To build project use scripts build.sh it will generate a os.bin file in bin folder which represents a virtual harddisk with OS kernel in it and QEMU emulator to laod that file in it and run

## The kernel

The kernel is a microkernel to the greatest degree possible, but includes drivers for printing  output to a screen , memory management(not optimised), minimum timer support for multitasking. It is a work in progress, and is currently stricly single-processor (no locking or synchronization is included, and the scheduler is primitive).

Memory Management:- For now its whole physical memory is divided into 4096 bytes of blocks and it allocates that whole block when a request for memory is made to kernel as lib.c is not included in this kernel

File System:- Its has Virtual Filesystem Layer to add support for more filesystem in future but for now it only support  FAT 16 filesystem on hardsisk 

Processes Management:- It have ability to switch Processes and save their context and schedule them using round robin Algorithm with minimum timer support required for that

## How it Works

Their are 3 processes that are located in programs folder and these processes does only one thing print to screen in infinite loop 
and for now these processes get compiled with the kernel and placed in binary disk with the kernel and when Qmeu loads that binary files bios see the first sector as bootloader and loads it and make jump to it then bootloader loads next 100 sectors of disk in memory as they are made reserved only for Kernel and make jump to that code and now control is transferred to kernel now kernel loads those 3 complied programs from disk and jump to usermode and start executing processes these  just write to screen and as kernel switches process we can see on screen different output of each process 
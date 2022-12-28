# efi-os
Unnamed 64bit x86 Hobby operating system with a custom uefi bootloader\
written in gnu c99 with nasm x64 assembly where necessary\
gnu-efi - Bootloader library and implementation\
kernel - kernel code\
OVMFbin - uefi files for qemu\
programs - programs to run inside the os\

## Building and running
### before first build run
	make gnu-efi 

### compile with
	make
### automatically run with qemu with
	make qemu

requres a relatively recent gcc(~9+, with bunutils), nasm, and make for compilation, mtools for disk image creation\
optionally requires qemu or virualbox for execution
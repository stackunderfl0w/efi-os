.PHONY: all kernel gnu-efi bootloader clean boot qemu virt

BL_DIR:= gnu-efi/bootloader
BL_BIN:= gnu-efi/x86_64/bootloader
KER_DIR:= kernel/bin

all: kernel bootloader boot
	
kernel:
	$(MAKE) -C kernel
gnu-efi:
	$(MAKE) -C gnu-efi
bootloader:
	$(MAKE) -C gnu-efi bootloader
clean:
	$(MAKE) -C kernel clean
	#$(MAKE) -C gnu-efi bootloader clean
	rm -f fat.img virtual_disk.vdi

boot: kernel bootloader
	rm -f fat.img
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img $(BL_BIN)/main.efi ::/EFI/BOOT/BOOTX64.efi
	mcopy -i fat.img $(BL_DIR)/startup.nsh ::/
	mcopy -i fat.img $(BL_DIR)/dat ::/resources
	mcopy -i fat.img $(KER_DIR)/kernel.elf ::/
	#mmd -i fat.img ::/resources/resourcesresources
	mcopy -i fat.img $(BL_DIR)/dat/resourcesresources ::/resources/resourcesresources

	#mcopy -i fat.img programs/scrclr/bin/scrclr.elf ::/resources/resourcesresources/

qemu:
	qemu-system-x86_64 -drive file=fat.img -m 256M -cpu qemu64 -smp 2 -drive if=pflash,format=raw,unit=0,file=OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=OVMFbin/OVMF_VARS-pure-efi.fd -net none #-no-reboot #-d int,cpu_reset
qemu-debug:
	qemu-system-x86_64 -drive file=fat.img -m 256M -cpu qemu64 -smp 2 -drive if=pflash,format=raw,unit=0,file=OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=OVMFbin/OVMF_VARS-pure-efi.fd -net none -no-reboot -d int,cpu_reset

	#-machine q35
	#qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none

virt:
	rm -f virtual_disk.vdi
	VBoxManage convertfromraw fat.img virtual_disk.vdi
	VBoxManage internalcommands sethduuid virtual_disk.vdi {e5976d35-70f1-4d25-bd18-88ebcad38805}
	VBoxManage startvm efios
.PHONY: all kernel bootloader gnu-efi $(PROGRAMS) clean boot qemu virt

BL_DIR:= gnu-efi/bootloader
BL_BIN:= gnu-efi/x86_64/bootloader
KER_DIR:= kernel/bin

PROGRAMS = $(notdir $(wildcard programs/*))

all: kernel bootloader boot
	
kernel:
	$(MAKE) -C kernel
gnu-efi:
	$(MAKE) -C gnu-efi
bootloader:
	$(MAKE) -C gnu-efi bootloader

$(PROGRAMS):
	$(MAKE) -C programs/$@

boot: kernel bootloader $(PROGRAMS)
	rm -f fat.img
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img $(BL_BIN)/main.efi ::/EFI/BOOT/BOOTX64.efi
	mcopy -i fat.img $(BL_DIR)/startup.nsh ::/
	mcopy -i fat.img $(BL_DIR)/dat ::/resources
	mcopy -i fat.img $(KER_DIR)/kernel.elf ::/
	for exe in $(PROGRAMS); do \
		mcopy -i fat.img "programs/$$exe/bin/$$exe.elf" ::/resources/ ;\
	done
	mcopy -i fat.img $(BL_DIR)/dat/resourcesresources ::/resources/resourcesresources
	@echo "Kernel built"

qemu: boot
	qemu-system-x86_64 -drive file=fat.img -m 256M -cpu qemu64 -smp 2 -drive if=pflash,format=raw,unit=0,file=OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=OVMFbin/OVMF_VARS-pure-efi.fd -net none #-no-reboot #-d int,cpu_reset
qemu-debug: boot
	qemu-system-x86_64 -drive file=fat.img -m 256M -cpu qemu64 -smp 2 -drive if=pflash,format=raw,unit=0,file=OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=OVMFbin/OVMF_VARS-pure-efi.fd -net none -no-reboot -d int,cpu_reset
qemu-gdb: boot
	qemu-system-x86_64 -s -S -drive file=fat.img -m 256M -cpu qemu64 -smp 2 -drive if=pflash,format=raw,unit=0,file=OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=OVMFbin/OVMF_VARS-pure-efi.fd -net none -no-reboot -d int,cpu_reset

	#-machine q35
	#qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none

virt: boot
	rm -f virtual_disk.vdi
	VBoxManage convertfromraw fat.img virtual_disk.vdi
	VBoxManage internalcommands sethduuid virtual_disk.vdi {e5976d35-70f1-4d25-bd18-88ebcad38805}
	VBoxManage startvm efios
	
clean:
	$(MAKE) -C kernel clean
	for exe in $(PROGRAMS); do \
		$(MAKE) -C programs/$$exe clean ;\
	done	
	rm -f fat.img virtual_disk.vdi

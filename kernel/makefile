GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld

SRC_DIR := src
OBJ_DIR := lib
BUILD_DIR := bin
DIRS := $(SRC_DIR) $(OBJ_DIR) $(BUILD_DIR)

SRC_FILES_C := $(wildcard $(SRC_DIR)/*.c)
SRC_FILES_ASM := $(wildcard $(SRC_DIR)/*.asm)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES_C))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(SRC_FILES_ASM))

CFLAGS := -g -Wall -std=gnu99 -ffreestanding -fno-builtin -mno-red-zone -fno-omit-frame-pointer -fno-stack-protector -Ofast -mtune=native
LDFLAGS:= -T $(LDS) -static -Bsymbolic -nostdlib

all: kernel

kernel: directories $(OBJ_FILES)
	ld $(LDFLAGS) -o $(BUILD_DIR)/kernel.elf $(OBJ_FILES)

$(OBJ_DIR)/interrupt_handlers.o: $(SRC_DIR)/interrupt_handlers.c
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@
$(OBJ_DIR)/loop.o: $(SRC_DIR)/loop.c
	$(CC) -ffreestanding -c $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	$(ASMC) $^ -f elf64 -o $@

directories:
	@mkdir -p bin lib src

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.so $(OBJ_DIR)/*.o
	rm -f $(BUILD_DIR)/*.elf

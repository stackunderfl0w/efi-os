GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld
EXECUTABLE = syscall_test

SRC_DIR := src
OBJ_DIR := lib
BUILD_DIR := bin
DIRS := $(SRC_DIR) $(OBJ_DIR) $(BUILD_DIR)

SRC_FILES_C := $(wildcard $(SRC_DIR)/*.c)
SRC_FILES_CPP := $(wildcard $(SRC_DIR)/*.cpp)
SRC_FILES_ASM := $(wildcard $(SRC_DIR)/*.asm)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES_C))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES_CPP))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(SRC_FILES_ASM))

CFLAGS := -Wall -std=gnu11 -ffreestanding -fno-builtin -mno-red-zone -fno-omit-frame-pointer -fno-stack-protector -Ofast -mtune=native

LDFLAGS:= -T $(LDS) -static -Bsymbolic -nostdlib

all: program

program: directories $(OBJ_FILES)
	ld $(LDFLAGS) -o $(BUILD_DIR)/$(EXECUTABLE).elf $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	gcc $(CFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	gcc $(CFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	nasm $^ -f elf64 -o $@

directories:
	@mkdir -p $(DIRS)

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.so $(OBJ_DIR)/*.o
	rm -f $(BUILD_DIR)/*.elf

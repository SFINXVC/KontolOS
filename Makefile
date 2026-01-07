# ============================================================================
# KontolOS Makefile
# ============================================================================
# Build system for KontolOS - A hobby operating system from scratch
# ============================================================================

# Detect if running in Docker (use native gcc with -m32)
ifdef DOCKER_BUILD
	CC = gcc
	LD = ld
	OBJCOPY = objcopy
else
	# Try cross-compiler first, fall back to native gcc
	CC = $(shell command -v i686-elf-gcc 2>/dev/null || echo gcc)
	LD = $(shell command -v i686-elf-ld 2>/dev/null || echo ld)
	OBJCOPY = $(shell command -v i686-elf-objcopy 2>/dev/null || echo objcopy)
endif

AS = nasm

# Directories
SRC_DIR = src
BUILD_DIR = build
BOOT_DIR = $(SRC_DIR)/boot
KERNEL_DIR = $(SRC_DIR)/kernel
DRIVERS_DIR = $(SRC_DIR)/drivers
LIB_DIR = $(SRC_DIR)/lib
FS_DIR = $(SRC_DIR)/fs

# Output files
OS_IMAGE = $(BUILD_DIR)/kontolos.img
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
STAGE1_BIN = $(BUILD_DIR)/stage1.bin
STAGE2_BIN = $(BUILD_DIR)/stage2.bin

# Compiler flags
CFLAGS = -m32 -ffreestanding -fno-exceptions \
         -nostdlib -fno-builtin -fno-stack-protector \
         -fno-pic -fno-pie \
         -Wall -Wextra \
         -I$(SRC_DIR)/include -I$(SRC_DIR)/kernel -I$(SRC_DIR)/drivers -I$(SRC_DIR)/lib

# Assembler flags
ASFLAGS_16 = -f bin
ASFLAGS_32 = -f elf32

# Linker flags
LDFLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld -nostdlib

# Source files
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c \
               $(KERNEL_DIR)/idt.c \
               $(KERNEL_DIR)/memory.c \
               $(KERNEL_DIR)/shell.c

DRIVER_C_SRC = $(DRIVERS_DIR)/vga.c \
               $(DRIVERS_DIR)/keyboard.c \
               $(DRIVERS_DIR)/timer.c

LIB_C_SRC = $(LIB_DIR)/string.c

FS_C_SRC = $(FS_DIR)/ramfs.c

# Object files
KERNEL_OBJ = $(BUILD_DIR)/kernel/kernel_entry.o \
             $(BUILD_DIR)/kernel/isr.o \
             $(BUILD_DIR)/kernel/kernel.o \
             $(BUILD_DIR)/kernel/idt.o \
             $(BUILD_DIR)/kernel/memory.o \
             $(BUILD_DIR)/kernel/shell.o

DRIVER_OBJ = $(BUILD_DIR)/drivers/vga.o \
             $(BUILD_DIR)/drivers/keyboard.o \
             $(BUILD_DIR)/drivers/timer.o

LIB_OBJ = $(BUILD_DIR)/lib/string.o

FS_OBJ = $(BUILD_DIR)/fs/ramfs.o

ALL_OBJ = $(KERNEL_OBJ) $(DRIVER_OBJ) $(LIB_OBJ) $(FS_OBJ)

# Default target
.PHONY: all
all: dirs $(OS_IMAGE)

# Docker build target
.PHONY: docker-build
docker-build: DOCKER_BUILD=1
docker-build: all

# Create build directories
.PHONY: dirs
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot
	@mkdir -p $(BUILD_DIR)/kernel
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/lib
	@mkdir -p $(BUILD_DIR)/fs

# Build Stage 1 bootloader
$(STAGE1_BIN): $(BOOT_DIR)/stage1.asm | dirs
	$(AS) $(ASFLAGS_16) $< -o $@

# Build Stage 2 bootloader
$(STAGE2_BIN): $(BOOT_DIR)/stage2.asm | dirs
	$(AS) $(ASFLAGS_16) $< -o $@

# Compile kernel assembly files
$(BUILD_DIR)/kernel/kernel_entry.o: $(KERNEL_DIR)/kernel_entry.asm | dirs
	$(AS) $(ASFLAGS_32) $< -o $@

$(BUILD_DIR)/kernel/isr.o: $(KERNEL_DIR)/isr.asm | dirs
	$(AS) $(ASFLAGS_32) $< -o $@

# Compile kernel C files
$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Compile driver C files
$(BUILD_DIR)/drivers/%.o: $(DRIVERS_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Compile library C files
$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Compile filesystem C files
$(BUILD_DIR)/fs/%.o: $(FS_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(BUILD_DIR)/kernel.elf: $(ALL_OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

# Convert kernel ELF to binary
$(KERNEL_BIN): $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

# Create the final OS image
$(OS_IMAGE): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@echo "Creating disk image..."
	@# Create empty disk image (1.44MB floppy)
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	@# Write Stage 1 bootloader (sector 0)
	dd if=$(STAGE1_BIN) of=$@ bs=512 count=1 conv=notrunc 2>/dev/null
	@# Write Stage 2 bootloader (sectors 1-16)
	dd if=$(STAGE2_BIN) of=$@ bs=512 seek=1 conv=notrunc 2>/dev/null
	@# Write kernel (starting at sector 17)
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=17 conv=notrunc 2>/dev/null
	@echo "OS image created: $@ ($(shell stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes)"

# Run in QEMU
.PHONY: run
run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -boot a

# Run in QEMU with debug options
.PHONY: debug
debug: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -boot a -s -S

# Clean build files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Print help
.PHONY: help
help:
	@echo "KontolOS Build System"
	@echo "====================="
	@echo "Targets:"
	@echo "  all          - Build the OS image (default)"
	@echo "  docker-build - Build using Docker environment"
	@echo "  run          - Build and run in QEMU"
	@echo "  debug        - Build and run in QEMU with GDB server"
	@echo "  clean        - Remove build files"
	@echo "  help         - Show this help message"

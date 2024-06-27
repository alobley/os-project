ASM=nasm
CCOM=gcc
ARCH=x86_64

CFLAGS=-O2 -nostdlib
LFLAGS=-O2 -nostdlib

EMARGS=-m 5G -smp 2 -vga std -device virtio-gpu-pci -serial stdio -drive file=build/main.img,format=raw,id=hd0,if=none -device virtio-blk-pci,drive=hd0

SRC_DIR=src
BUILD_DIR=build
LIB_DIR=src/lib

LIBS=$(LIB_DIR)/memory.c $(LIB_DIR)/pci.c $(LIB_DIR)/virtio.c $(LIB_DIR)/graphics.c $(LIB_DIR)/io.c

ASMFILE=boot64
CFILE=kernel

# Automatically call everything in the order needed
all: assemble compile drive_image qemu clean

#
# Create Boot Disk
#
drive_image: $(BUILD_DIR)/main.img
$(BUILD_DIR)/main.img: assemble compile
	dd if=/dev/zero of=$(BUILD_DIR)/main.img bs=512 count=10000
	dd if=$(BUILD_DIR)/$(ASMFILE).bin of=$(BUILD_DIR)/main.img conv=notrunc
	dd if=$(BUILD_DIR)/$(CFILE).bin of=$(BUILD_DIR)/main.img bs=512 seek=1 conv=notrunc

#
# Assemble
#
assemble: $(SRC_DIR)/$(ASMFILE).asm $(SRC_DIR)/kernel_start.s
	$(ASM) $(SRC_DIR)/$(ASMFILE).asm -o $(BUILD_DIR)/$(ASMFILE).bin
	$(CCOM) -c $(SRC_DIR)/kernel_start.s -o $(BUILD_DIR)/kernel_start.o $(CFLAGS)

#
# Compile
#
compile: $(SRC_DIR)/$(CFILE).c $(BUILD_DIR)/kernel_start.o
	$(CCOM) -o $(BUILD_DIR)/$(CFILE).elf -T linker.ld $(SRC_DIR)/kernel_start.s $(SRC_DIR)/kernel.c $(LIBS) $(CFLAGS)
	objcopy -O binary $(BUILD_DIR)/$(CFILE).elf $(BUILD_DIR)/$(CFILE).bin

#
# Run
# 
qemu: $(BUILD_DIR)/main.img
	qemu-system-$(ARCH) $(EMARGS)

#
# Clean
#
clean:
	rm -rf build/*


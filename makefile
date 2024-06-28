ASM=nasm
CCOM=gcc
ARCH=x86_64

CFLAGS=-O2 -nostdlib -ffreestanding -fno-stack-protector -no-pie -T linker.ld -std=gnu99

EMARGS=-m 5G -smp 2 -vga std -device virtio-gpu-pci -serial stdio -drive file=build/main.img,format=raw,id=hd0,if=none -device virtio-blk-pci,drive=hd0

SRC_DIR=src
BUILD_DIR=build
LIB_DIR=src/lib
INT_DIR=src/lib/interrupts

LIBS=$(LIB_DIR)/memory.c $(LIB_DIR)/pci.c $(LIB_DIR)/virtio.c $(LIB_DIR)/graphics.c $(LIB_DIR)/io.c $(LIB_DIR)/keyboard.c $(LIB_DIR)/time.c $(LIB_DIR)/vga.c 
LIBS+=$(BUILD_DIR)/intasm.o $(INT_DIR)/pic.c $(INT_DIR)/idt.c $(INT_DIR)/exceptions.c $(INT_DIR)/interrupts.c $(INT_DIR)/ioapic.c $(INT_DIR)/localapic.c

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
assemble: $(SRC_DIR)/$(ASMFILE).asm
	$(ASM) $(SRC_DIR)/$(ASMFILE).asm -o $(BUILD_DIR)/$(ASMFILE).bin

#
# Compile
#
compile: $(SRC_DIR)/$(CFILE).c
	$(ASM) -felf64 $(INT_DIR)/interrupts.asm -o $(BUILD_DIR)/intasm.o
	$(CCOM) -o $(BUILD_DIR)/$(CFILE).bin $(SRC_DIR)/$(CFILE).c $(LIBS) $(CFLAGS)

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


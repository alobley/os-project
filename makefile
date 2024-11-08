ASM=nasm
CCOM=i686-elf-gcc
ARCH=i386

EMARGS=-m 4G -smp 1 -vga std -serial stdio -drive file=build/main.iso,media=cdrom,if=ide -drive file=bin/harddisk.vdi,format=raw,if=ide -boot d
SRC_DIR=src
BUILD_DIR=build
LIB_DIR=src/lib
INT_DIR=src/interrupts
VGA_DIR=src/VGA
BOOT_DIR=src/boot
KERNEL_DIR=src/kernel
MEM_DIR=src/memory
TIME_DIR=src/time
KB_DIR=src/keyboard

INCLUDES=-I $(SRC_DIR) -I $(LIB_DIR) -I $(INT_DIR) -I $(VGA_DIR) -I $(BOOT_DIR) -I $(KERNEL_DIR) -I $(MEM_DIR) -I $(TIME_DIR) -I $(KB_DIR)

CFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib --std=gnu99 -Wall $(INCLUDES)

LIBS=$(BUILD_DIR)/kernel_start.o $(INT_DIR)/isr.c $(INT_DIR)/idt.c $(INT_DIR)/irq.c $(LIB_DIR)/io.c $(LIB_DIR)/fpu.c $(VGA_DIR)/vga.c $(VGA_DIR)/pixel.c $(VGA_DIR)/text.c 
LIBS+=$(MEM_DIR)/alloc.c $(TIME_DIR)/time.c $(KB_DIR)/keyboard.c

ASMFILE=boot
CFILE=kernel
PROGRAM_FILE=programtoload

# Automatically call everything in the order needed
all: assemble compile drive_image qemu

#
# Create Boot Disk
#
drive_image:
$(BUILD_DIR)/main.img: assemble compile
	mkdir -p isodir/boot/grub
	cp $(BUILD_DIR)/$(CFILE).bin isodir/boot/$(CFILE).bin
	cp $(BOOT_DIR)/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/main.iso isodir

#
# Assemble
#
assemble:
	$(ASM) -felf32 $(KERNEL_DIR)/kernel_start.asm -o $(BUILD_DIR)/kernel_start.o

#
# Compile
#
compile: $(KERNEL_DIR)/$(CFILE).c
	$(CCOM) -o $(BUILD_DIR)/$(CFILE).bin $(KERNEL_DIR)/$(CFILE).c $(LIBS) $(CFLAGS)

#
# Run
# 
qemu: $(BUILD_DIR)/main.img
	qemu-system-$(ARCH) $(EMARGS)

addfiles:
	mkdir -p mnt
	sudo mount -o loop,rw bin/harddisk.vdi mnt
	sudo cp $(BUILD_DIR)/prgm.bin mnt/prgm.bin
	sync
	sudo umount mnt

# Because for some reason .img is write protected.
hard_drive:
	mkdir -p bin
	qemu-img create -f raw bin/harddisk.vdi 2G
	mkfs.fat -F 32 bin/harddisk.vdi

#
# Clean
#
clean:
	rm -rf build/*
	rm -rf isodir/*
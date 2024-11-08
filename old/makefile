ASM=nasm
CCOM=i686-elf-gcc
ARCH=i386

CFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib --std=gnu99 -Wall

EMARGS=-m 4G -smp 1 -device vmware-svga,vgamem_mb=256 -serial stdio -drive file=build/main.iso,media=cdrom,if=ide -drive file=bin/harddisk.vdi,format=raw,if=ide -boot d
SRC_DIR=src
BUILD_DIR=build
LIB_DIR=src/lib
INT_DIR=src/lib/interrupts
DRIVER_DIR=src/lib/drivers

LIBS=$(BUILD_DIR)/kernel_start.o $(DRIVER_DIR)/console.c $(LIB_DIR)/io.c $(LIB_DIR)/memory.c $(DRIVER_DIR)/keyboard.c $(LIB_DIR)/fpu.c $(DRIVER_DIR)/pci.c
LIBS+=$(INT_DIR)/isr.c $(INT_DIR)/idt.c $(INT_DIR)/irq.c $(LIB_DIR)/time.c $(DRIVER_DIR)/graphics.c $(LIB_DIR)/math.c $(DRIVER_DIR)/ata.c $(DRIVER_DIR)/fat32.c
LIBS+=$(LIB_DIR)/string.c

ASMFILE=boot
CFILE=kernel
PROGRAM_FILE=programtoload

# Automatically call everything in the order needed
all: assemble compile drive_image addfiles qemu

#
# Create Boot Disk
#
drive_image:
$(BUILD_DIR)/main.img: assemble compile
	mkdir -p isodir/boot/grub
	cp $(BUILD_DIR)/$(CFILE).bin isodir/boot/$(CFILE).bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/main.iso isodir

#
# Assemble
#
assemble: $(SRC_DIR)/$(ASMFILE).asm
	$(ASM) -f bin $(SRC_DIR)/$(PROGRAM_FILE).asm -o $(BUILD_DIR)/$(PROGRAM_FILE).bin
	$(ASM) -f bin $(SRC_DIR)/programtoload.asm -o $(BUILD_DIR)/prgm.bin

#
# Compile
#
compile: $(SRC_DIR)/$(CFILE).c
	$(ASM) -felf32 $(SRC_DIR)/kernel_start.asm -o $(BUILD_DIR)/kernel_start.o
	$(CCOM) -o $(BUILD_DIR)/$(CFILE).bin $(SRC_DIR)/$(CFILE).c $(LIBS) $(CFLAGS)

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


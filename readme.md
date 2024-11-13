# My custom OS project (name undecided)

This project is meant to be a lightweight 32-bit x86 operating system developed entirely and exclusively by me, using only my own code. Every line of code in every file was written by me, unless stated otherwise. GRUB, of course, also does not belong to me. Written entirely from scratch.

## Project Status
Currently, this project is still in the early stages. I've spent the past year learning about x86 and I started with basically zero knowledge, including in C and assembly.

## Features
- **Boot Process**:
  - Boots with GRUB
  - Installs the Global Descriptor Table (GDT)
- **System Initialization**:
  - Initializes memory management
  - Sets up interrupt handling
- **Hardware Support**:
  - Keyboard and timer ISRs mapped to the Programmable Interrupt Controller (PIC)
  - PC speaker initialization
- **Graphics**:
  - VGA driver setup
- **Kernel**:
  - Executes minimal kernel functionality



## To-Do
1. Comment and document the code. Both are very sparse.
2. Learn how to use git and GitHub properly.
3. Write an ATA and FAT driver for disk access.
4. Write a system call interrupt and an ABI.

## Long-Term Goals
5. Write a terminal application using my custom ABI.
6. Implement paging and better memory allocation.
7. Implement process management and multitasking.
8. Add a userland and memory protection.
9. Add USB support.
10. Add milticore support.

## Build & Run

**Note:** This is currently untested on real hardware. It is recommended to use QEMU for emulation.

### Prerequisites
Ensure you have the following installed:
- A Linux or FreeBSD system
- `i686-elf-gcc` (cross-compiler)
- `i686-elf-ld`
- `binutils`
- `NASM`
- `QEMU`

### Build Instructions
1. Clone the repository:
   ```bash```
   git clone https://github.com/alobley/os-project.git
2. Change directory:
    cd os-project
3. Build and run the project:
    make

## Contributing
Feel free to fork this if you wish to do so, but the goal of this project is to see what I can do if everything is supported by my own code. Feedback, however, is always welcome.

## License
This project is dual-licensed:
- The **GRand Unified Bootloader (GRUB)**, which is used to load the OS, is under the GNU General Public License (GPL).
- All other parts of the project are licensed under the **MIT License**. See the `LICENSE` file in the root directory for more information.

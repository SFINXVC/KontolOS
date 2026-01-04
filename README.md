# KontolOS

A hobby operating system built completely from scratch using C and Assembly.

```
  _  __            _        _  ___  ____  
 | |/ /___  _ __ | |_ ___ | |/ _ \/ ___| 
 | ' // _ \| '_ \| __/ _ \| | | | \___ \ 
 | . \ (_) | | | | || (_) | | |_| |___) |
 |_|\_\___/|_| |_|\__\___/|_|\___/|____/ 
```

## Features

- **Custom Bootloader**: Two-stage bootloader written in Assembly
  - Stage 1: MBR bootloader (512 bytes) that loads Stage 2
  - Stage 2: Protected mode setup, A20 line, GDT configuration
- **32-bit Protected Mode Kernel**: Written in C
- **VGA Text Mode Driver**: 80x25 color text display
- **Interrupt Handling**: Full IDT with exception and IRQ handlers
- **Keyboard Driver**: PS/2 keyboard with US QWERTY layout
- **Timer Driver**: PIT-based system timer
- **Memory Manager**: Simple heap allocator
- **Interactive Shell**: Command-line interface with multiple commands

## Shell Commands

| Command    | Description                      |
|------------|----------------------------------|
| `help`     | Display available commands       |
| `clear`    | Clear the screen                 |
| `echo`     | Echo text to the screen          |
| `info`     | Display system information       |
| `uptime`   | Show system uptime               |
| `memory`   | Display memory statistics        |
| `reboot`   | Reboot the system                |
| `halt`     | Halt the system                  |
| `version`  | Display OS version               |
| `color`    | Change text color (0-15)         |

## Prerequisites

You'll need the following tools installed:

1. **NASM** (Netwide Assembler)
   - Download: https://nasm.us/
   - Or install via: `choco install nasm`

2. **i686-elf Cross-Compiler**
   - Download prebuilt: https://github.com/lordmilko/i686-elf-tools/releases
   - Extract and add to PATH

3. **QEMU** (for testing)
   - Download: https://qemu.org/download/
   - Or install via: `choco install qemu`

## Building

### Windows (with cross-compiler)

```batch
.\build.bat
```

### Windows (with Docker)

If you don't have the cross-compiler installed, you can use Docker:

```batch
.\build-docker.bat
```

### Linux/macOS (with Make)

```bash
make
```

## Running

After building, run the OS in QEMU:

```batch
.\run.bat
```

Or manually:

```batch
qemu-system-i386 -fda build\kontolos.img
```

## Project Structure

```
KontolOS/
├── src/
│   ├── boot/
│   │   ├── stage1.asm      # Stage 1 bootloader (MBR)
│   │   └── stage2.asm      # Stage 2 bootloader
│   ├── kernel/
│   │   ├── kernel_entry.asm # Kernel entry point
│   │   ├── kernel.c         # Main kernel
│   │   ├── kernel.h         # Kernel header
│   │   ├── idt.c            # Interrupt Descriptor Table
│   │   ├── idt.h
│   │   ├── isr.asm          # ISR/IRQ assembly stubs
│   │   ├── memory.c         # Memory manager
│   │   ├── memory.h
│   │   ├── shell.c          # Interactive shell
│   │   └── shell.h
│   ├── drivers/
│   │   ├── vga.c            # VGA text mode driver
│   │   ├── vga.h
│   │   ├── keyboard.c       # Keyboard driver
│   │   ├── keyboard.h
│   │   ├── timer.c          # PIT timer driver
│   │   └── timer.h
│   ├── lib/
│   │   ├── string.c         # String functions
│   │   └── string.h
│   └── linker.ld            # Kernel linker script
├── build/                    # Build output directory
├── Makefile                  # Make build system
├── build.bat                 # Windows build script
├── build-docker.bat          # Docker build script
├── create_image.ps1          # PowerShell image creator
├── run.bat                   # QEMU launcher
└── README.md
```

## How It Works

### Boot Process

1. **BIOS** loads the first 512 bytes (Stage 1) from the disk to `0x7C00`
2. **Stage 1** sets up segments and loads Stage 2 from disk
3. **Stage 2**:
   - Enables the A20 line (access to memory > 1MB)
   - Loads the kernel to temporary memory
   - Sets up the Global Descriptor Table (GDT)
   - Switches the CPU to 32-bit Protected Mode
   - Copies kernel to 1MB
   - Jumps to the kernel entry point
4. **Kernel** initializes:
   - VGA text mode driver
   - Interrupt Descriptor Table (IDT)
   - Timer (PIT at 100Hz)
   - Keyboard driver
   - Memory manager
   - Starts the shell

### Memory Layout

| Address Range       | Description              |
|---------------------|--------------------------|
| `0x00000 - 0x07BFF` | Real mode IVT and BIOS   |
| `0x07C00 - 0x07DFF` | Stage 1 bootloader       |
| `0x10000 - 0x11FFF` | Stage 2 bootloader       |
| `0x20000 - 0x2FFFF` | Temporary kernel load    |
| `0x90000 - 0x9FFFF` | Stack                    |
| `0xB8000 - 0xB8FFF` | VGA text buffer          |
| `0x100000+`         | Kernel (at 1MB)          |
| `0x200000+`         | Heap (4MB)               |

## License

This project is open source and available under the MIT License.

## Acknowledgments

- OSDev Wiki (https://wiki.osdev.org/)
- James Molloy's Kernel Development Tutorials
- Bran's Kernel Development Tutorial

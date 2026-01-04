@echo off
REM ============================================================================
REM KontolOS Build Script for Windows
REM ============================================================================
REM This script builds the OS using native Windows tools and/or Docker
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================
echo  KontolOS Build System for Windows
echo ========================================
echo.

REM Check for required tools
set MISSING_TOOLS=0

REM Check for NASM
where nasm >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] NASM not found. Please install NASM from https://nasm.us/
    set MISSING_TOOLS=1
) else (
    echo [+] NASM found
)

REM Check for QEMU
where qemu-system-i386 >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] QEMU not found. Please install QEMU from https://qemu.org/
    set MISSING_TOOLS=1
) else (
    echo [+] QEMU found
)

REM Check for cross-compiler (or suggest alternatives)
where i686-elf-gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] i686-elf-gcc cross-compiler not found.
    echo     Options:
    echo     1. Install cross-compiler from: https://github.com/lordmilko/i686-elf-tools/releases
    echo     2. Use Docker (see build-docker.bat)
    echo     3. Use WSL
    set MISSING_TOOLS=1
) else (
    echo [+] Cross-compiler found
)

if %MISSING_TOOLS% EQU 1 (
    echo.
    echo [!] Some required tools are missing. See messages above.
    echo [!] You can also use build-docker.bat to build using Docker.
    echo.
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
if not exist build\boot mkdir build\boot
if not exist build\kernel mkdir build\kernel
if not exist build\drivers mkdir build\drivers
if not exist build\lib mkdir build\lib

echo.
echo [*] Building bootloaders...

REM Build Stage 1 bootloader
nasm -f bin src\boot\stage1.asm -o build\stage1.bin
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to build Stage 1 bootloader
    exit /b 1
)
echo [+] Stage 1 bootloader built

REM Build Stage 2 bootloader
nasm -f bin src\boot\stage2.asm -o build\stage2.bin
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to build Stage 2 bootloader
    exit /b 1
)
echo [+] Stage 2 bootloader built

echo.
echo [*] Building kernel...

REM Build kernel assembly files
nasm -f elf32 src\kernel\kernel_entry.asm -o build\kernel\kernel_entry.o
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to build kernel_entry.asm
    exit /b 1
)

nasm -f elf32 src\kernel\isr.asm -o build\kernel\isr.o
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to build isr.asm
    exit /b 1
)

REM Build kernel C files
for %%f in (src\kernel\*.c) do (
    set BASENAME=%%~nf
    echo [*] Compiling %%f...
    i686-elf-gcc -m32 -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -c %%f -o build\kernel\!BASENAME!.o -Isrc\kernel -Isrc\drivers -Isrc\lib
    if !ERRORLEVEL! NEQ 0 (
        echo [!] Failed to compile %%f
        exit /b 1
    )
)

REM Build driver C files
for %%f in (src\drivers\*.c) do (
    set BASENAME=%%~nf
    echo [*] Compiling %%f...
    i686-elf-gcc -m32 -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -c %%f -o build\drivers\!BASENAME!.o -Isrc\kernel -Isrc\drivers -Isrc\lib
    if !ERRORLEVEL! NEQ 0 (
        echo [!] Failed to compile %%f
        exit /b 1
    )
)

REM Build library C files
for %%f in (src\lib\*.c) do (
    set BASENAME=%%~nf
    echo [*] Compiling %%f...
    i686-elf-gcc -m32 -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -c %%f -o build\lib\!BASENAME!.o -Isrc\kernel -Isrc\drivers -Isrc\lib
    if !ERRORLEVEL! NEQ 0 (
        echo [!] Failed to compile %%f
        exit /b 1
    )
)

echo [+] Kernel compiled

echo.
echo [*] Linking kernel...

REM Link kernel
i686-elf-ld -m elf_i386 -T src\linker.ld -nostdlib build\kernel\kernel_entry.o build\kernel\isr.o build\kernel\kernel.o build\kernel\idt.o build\kernel\memory.o build\kernel\shell.o build\drivers\vga.o build\drivers\keyboard.o build\drivers\timer.o build\lib\string.o -o build\kernel.elf
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to link kernel
    exit /b 1
)

REM Convert to binary
i686-elf-objcopy -O binary build\kernel.elf build\kernel.bin
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to convert kernel to binary
    exit /b 1
)

echo [+] Kernel linked

echo.
echo [*] Creating disk image...

REM Create the disk image using PowerShell
powershell -ExecutionPolicy Bypass -File create_image.ps1
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to create disk image
    exit /b 1
)

echo [+] Disk image created: build\kontolos.img

echo.
echo ========================================
echo  Build Complete!
echo ========================================
echo.
echo To run the OS:
echo   qemu-system-i386 -fda build\kontolos.img
echo.
echo Or run: run.bat
echo.

endlocal

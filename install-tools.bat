@echo off
REM ============================================================================
REM KontolOS Tool Installer for Windows
REM ============================================================================
REM This script helps install the required development tools
REM ============================================================================

echo.
echo ========================================
echo  KontolOS Tool Installer
echo ========================================
echo.
echo This script will help you install the required tools:
echo   1. NASM (Netwide Assembler)
echo   2. QEMU (x86 Emulator)
echo   3. i686-elf Cross-Compiler
echo.

REM Check for admin rights for Chocolatey
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] NOTE: Some installations may require Administrator privileges.
    echo.
)

REM Check if Chocolatey is installed
where choco >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [+] Chocolatey is installed. You can install tools with:
    echo     choco install nasm
    echo     choco install qemu
    echo.
    set /p INSTALL_CHOCO="Would you like to install NASM and QEMU now? (Y/N): "
    if /i "!INSTALL_CHOCO!"=="Y" (
        echo [*] Installing NASM...
        choco install nasm -y
        echo [*] Installing QEMU...
        choco install qemu -y
    )
    goto :cross_compiler
)

echo [*] Chocolatey not found. Manual installation instructions:
echo.
echo ========================================
echo  1. NASM (Required)
echo ========================================
echo Download from: https://nasm.us/pub/nasm/releasebuilds/
echo - Download the latest win64 installer
echo - Run the installer
echo - Add NASM to your PATH (e.g., C:\Program Files\NASM)
echo.
echo ========================================
echo  2. QEMU (Required for testing)
echo ========================================
echo Download from: https://qemu.weilnetz.de/w64/
echo - Download qemu-w64-setup-*.exe
echo - Run the installer
echo - Add QEMU to your PATH (e.g., C:\Program Files\qemu)
echo.

:cross_compiler
echo ========================================
echo  3. i686-elf Cross-Compiler (Required)
echo ========================================
echo.
echo Option A: Download pre-built binaries
echo   https://github.com/lordmilko/i686-elf-tools/releases
echo   - Download i686-elf-tools-windows.zip
echo   - Extract to a folder (e.g., C:\i686-elf)
echo   - Add the bin folder to your PATH
echo.
echo Option B: Use Docker (no cross-compiler needed)
echo   - Install Docker Desktop
echo   - Run: build-docker.bat
echo.
echo Option C: Use WSL (Windows Subsystem for Linux)
echo   - Install WSL with Ubuntu
echo   - Install: sudo apt install build-essential nasm qemu-system-x86
echo   - Install cross-compiler: sudo apt install gcc-i686-linux-gnu
echo   - Run: make
echo.

echo ========================================
echo  Setting up PATH (if tools are installed)
echo ========================================
echo.
echo After installing the tools, add them to your PATH:
echo.
echo 1. Open System Properties ^> Advanced ^> Environment Variables
echo 2. Under "User variables", edit "Path"
echo 3. Add the following paths:
echo    - C:\Program Files\NASM
echo    - C:\Program Files\qemu
echo    - C:\i686-elf\bin (or wherever you extracted the cross-compiler)
echo 4. Click OK and restart your terminal
echo.

pause

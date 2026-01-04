@echo off
REM Run KontolOS in QEMU

if not exist build\kontolos.img (
    echo [!] OS image not found. Run build-docker.bat first.
    pause
    exit /b 1
)

echo [*] Starting KontolOS in QEMU...

REM Try common QEMU locations
if exist "C:\Program Files\qemu\qemu-system-i386.exe" (
    "C:\Program Files\qemu\qemu-system-i386.exe" -fda build\kontolos.img -boot a
) else if exist "C:\Program Files (x86)\qemu\qemu-system-i386.exe" (
    "C:\Program Files (x86)\qemu\qemu-system-i386.exe" -fda build\kontolos.img -boot a
) else (
    REM Try from PATH
    qemu-system-i386 -fda build\kontolos.img -boot a
)

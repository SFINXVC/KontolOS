# KontolOS Disk Image Creator
# Creates a bootable disk image from the bootloader and kernel binaries

$ErrorActionPreference = "Stop"

$BuildDir = "build"
$ImagePath = "$BuildDir\kontolos.img"
$Stage1Path = "$BuildDir\stage1.bin"
$Stage2Path = "$BuildDir\stage2.bin"
$KernelPath = "$BuildDir\kernel.bin"

# Image size: 1.44MB floppy disk
$ImageSize = 512 * 2880

Write-Host "[*] Creating disk image..." -ForegroundColor Cyan

# Create empty image
$image = New-Object byte[] $ImageSize

# Read Stage 1 bootloader (512 bytes, sector 0)
if (Test-Path $Stage1Path) {
    $stage1 = [System.IO.File]::ReadAllBytes($Stage1Path)
    Write-Host "[*] Stage 1 size: $($stage1.Length) bytes"
    [Array]::Copy($stage1, 0, $image, 0, [Math]::Min($stage1.Length, 512))
} else {
    Write-Host "[!] Stage 1 bootloader not found: $Stage1Path" -ForegroundColor Red
    exit 1
}

# Read Stage 2 bootloader (8KB = 16 sectors, sectors 1-16)
if (Test-Path $Stage2Path) {
    $stage2 = [System.IO.File]::ReadAllBytes($Stage2Path)
    Write-Host "[*] Stage 2 size: $($stage2.Length) bytes"
    $stage2Offset = 512  # After boot sector
    [Array]::Copy($stage2, 0, $image, $stage2Offset, [Math]::Min($stage2.Length, 8192))
} else {
    Write-Host "[!] Stage 2 bootloader not found: $Stage2Path" -ForegroundColor Red
    exit 1
}

# Read kernel binary (starting at sector 17)
if (Test-Path $KernelPath) {
    $kernel = [System.IO.File]::ReadAllBytes($KernelPath)
    Write-Host "[*] Kernel size: $($kernel.Length) bytes"
    $kernelOffset = 512 * 17  # After Stage 2
    
    if ($kernelOffset + $kernel.Length -gt $ImageSize) {
        Write-Host "[!] Kernel too large for floppy image!" -ForegroundColor Yellow
        # Extend image size to accommodate kernel
        $newSize = $kernelOffset + $kernel.Length + 512
        $newImage = New-Object byte[] $newSize
        [Array]::Copy($image, 0, $newImage, 0, $image.Length)
        $image = $newImage
    }
    
    [Array]::Copy($kernel, 0, $image, $kernelOffset, $kernel.Length)
} else {
    Write-Host "[!] Kernel not found: $KernelPath" -ForegroundColor Red
    exit 1
}

# Write the image to disk
[System.IO.File]::WriteAllBytes($ImagePath, $image)

Write-Host "[+] Disk image created: $ImagePath ($($image.Length) bytes)" -ForegroundColor Green

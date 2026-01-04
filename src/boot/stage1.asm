; ============================================================================
; KontolOS Stage 1 Bootloader
; ============================================================================
; This is the first 512 bytes loaded by BIOS from the boot sector.
; It sets up the environment and loads Stage 2 bootloader.
; ============================================================================

[BITS 16]                       ; We start in 16-bit Real Mode
[ORG 0x7C00]                    ; BIOS loads boot sector at 0x7C00

; ============================================================================
; Constants
; ============================================================================
STAGE2_LOAD_SEGMENT     equ 0x1000      ; Segment where Stage 2 will be loaded
STAGE2_LOAD_OFFSET      equ 0x0000      ; Offset where Stage 2 will be loaded
STAGE2_SECTORS          equ 16          ; Number of sectors to load for Stage 2
STAGE2_START_SECTOR     equ 2           ; Stage 2 starts at sector 2 (1-indexed, sector 1 is boot)

; ============================================================================
; Entry Point
; ============================================================================
start:
    ; Disable interrupts during setup
    cli

    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00              ; Stack grows downward from boot sector

    ; Save boot drive number (BIOS passes it in DL)
    mov [boot_drive], dl

    ; Enable interrupts
    sti

    ; Clear screen
    call clear_screen

    ; Print welcome message
    mov si, msg_welcome
    call print_string

    ; Print loading message
    mov si, msg_loading
    call print_string

    ; Load Stage 2 bootloader
    call load_stage2

    ; Print success message
    mov si, msg_success
    call print_string

    ; Pass boot drive to Stage 2 via DL register
    mov dl, [boot_drive]

    ; Jump to Stage 2
    jmp STAGE2_LOAD_SEGMENT:STAGE2_LOAD_OFFSET

; ============================================================================
; Load Stage 2 Bootloader from disk
; ============================================================================
load_stage2:
    pusha

    ; Reset disk system
    xor ax, ax
    mov dl, [boot_drive]
    int 0x13
    jc .disk_error

    ; Set up for disk read
    mov ah, 0x02                ; BIOS read sectors function
    mov al, STAGE2_SECTORS      ; Number of sectors to read
    mov ch, 0                   ; Cylinder 0
    mov cl, STAGE2_START_SECTOR ; Starting sector
    mov dh, 0                   ; Head 0
    mov dl, [boot_drive]        ; Drive number

    ; Set destination buffer
    mov bx, STAGE2_LOAD_SEGMENT
    mov es, bx
    mov bx, STAGE2_LOAD_OFFSET

    ; Read sectors
    int 0x13
    jc .disk_error

    ; Verify correct number of sectors read
    cmp al, STAGE2_SECTORS
    jne .disk_error

    popa
    ret

.disk_error:
    mov si, msg_disk_error
    call print_string
    jmp hang

; ============================================================================
; Print a null-terminated string
; SI = pointer to string
; ============================================================================
print_string:
    pusha
    mov ah, 0x0E                ; BIOS teletype function
.loop:
    lodsb                       ; Load byte from SI into AL
    test al, al                 ; Check for null terminator
    jz .done
    int 0x10                    ; Print character
    jmp .loop
.done:
    popa
    ret

; ============================================================================
; Clear the screen
; ============================================================================
clear_screen:
    pusha
    mov ah, 0x00                ; Set video mode
    mov al, 0x03                ; 80x25 text mode
    int 0x10
    popa
    ret

; ============================================================================
; Hang forever (infinite loop)
; ============================================================================
hang:
    cli
    hlt
    jmp hang

; ============================================================================
; Data Section
; ============================================================================
boot_drive:         db 0

msg_welcome:        db '========================================', 13, 10
                    db '       KontolOS Bootloader v1.0         ', 13, 10
                    db '========================================', 13, 10, 0

msg_loading:        db '[*] Loading Stage 2...', 13, 10, 0
msg_success:        db '[+] Stage 2 loaded successfully!', 13, 10
                    db '[*] Jumping to Stage 2...', 13, 10, 0
msg_disk_error:     db '[!] DISK ERROR! System halted.', 13, 10, 0

; ============================================================================
; Boot Sector Padding and Signature
; ============================================================================
times 510 - ($ - $$) db 0       ; Pad to 510 bytes
dw 0xAA55                       ; Boot signature

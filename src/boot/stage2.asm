; ============================================================================
; KontolOS Stage 2 Bootloader
; ============================================================================

[BITS 16]
[ORG 0x0000]                    ; ORG 0, loaded at segment 0x1000

; ============================================================================
; Constants
; ============================================================================
KERNEL_LOAD_ADDR        equ 0x100000    ; 1MB mark
KERNEL_TEMP_SEG         equ 0x2000      ; 0x20000
KERNEL_TEMP_ADDR        equ 0x20000
KERNEL_SECTORS          equ 48          ; ~24KB for kernel

; ============================================================================
; Entry Point
; ============================================================================
stage2_start:
    ; Set up segments
    mov ax, 0x1000
    mov ds, ax
    mov es, ax  
    mov ss, ax
    mov sp, 0xFFFE

    ; Save boot drive
    mov [boot_drive], dl

    ; Print banner
    mov si, msg_stage2
    call print16

    ; Enable A20
    call enable_a20
    mov si, msg_a20
    call print16

    ; Load kernel
    mov si, msg_loading
    call print16
    call load_kernel
    mov si, msg_loaded
    call print16

    ; Setup GDT
    call setup_gdt
    mov si, msg_gdt
    call print16

    ; Enter protected mode
    mov si, msg_pmode
    call print16

    cli
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:(0x10000 + pmode_entry)

; ============================================================================
boot_drive: db 0

; ============================================================================
; Enable A20 via Fast Gate
; ============================================================================
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; ============================================================================
; Load kernel using simple loop
; ============================================================================
load_kernel:
    pusha
    
    mov ax, KERNEL_TEMP_SEG
    mov es, ax
    xor bx, bx                  ; ES:BX = 0x2000:0000

    mov cl, 18                  ; Start sector (1-indexed) - sector 18
    mov ch, 0                   ; Cylinder 0
    mov dh, 0                   ; Head 0
    
    mov si, KERNEL_SECTORS      ; Sectors to read

.loop:
    cmp si, 0
    je .done

    ; Read 1 sector at a time for reliability
    mov ah, 0x02
    mov al, 1
    mov dl, [boot_drive]
    
    push si
    mov di, 3
.retry:
    push ax
    push bx
    push cx
    push dx
    int 0x13
    jnc .ok
    
    ; Reset on error
    xor ax, ax
    int 0x13
    
    pop dx
    pop cx
    pop bx
    pop ax
    dec di
    jnz .retry
    jmp .error
    
.ok:
    pop dx
    pop cx
    pop bx
    pop ax
    pop si
    
    ; Advance buffer by 512 bytes (32 paragraphs)
    mov ax, es
    add ax, 0x20
    mov es, ax
    
    ; Next sector
    inc cl
    cmp cl, 19                  ; Past sector 18?
    jb .no_next_head
    mov cl, 1                   ; Back to sector 1
    inc dh                      ; Next head
    cmp dh, 2                   ; Past head 1?
    jb .no_next_head
    mov dh, 0                   ; Back to head 0
    inc ch                      ; Next cylinder
.no_next_head:
    
    dec si
    jmp .loop

.done:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax
    popa
    ret

.error:
    mov ax, 0x1000
    mov ds, ax
    mov si, msg_error
    call print16
    cli
    hlt

; ============================================================================
; Setup GDT
; ============================================================================
setup_gdt:
    ; Calculate linear address of GDT
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, gdt_start
    mov [gdt_ptr + 2], eax
    lgdt [gdt_ptr]
    ret

; ============================================================================
; GDT
; ============================================================================
align 8
gdt_start:
    dq 0                        ; Null descriptor
gdt_code:
    dw 0xFFFF, 0x0000           ; Limit, Base low
    db 0x00                     ; Base mid
    db 10011010b                ; Access
    db 11001111b                ; Flags + Limit high
    db 0x00                     ; Base high
gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1
    dd 0                        ; Filled at runtime

CODE_SEG equ 0x08
DATA_SEG equ 0x10

; ============================================================================
; Print (16-bit)
; ============================================================================
print16:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; ============================================================================
; Messages
; ============================================================================
msg_stage2:  db 13,10,'[Stage 2] Started',13,10,0
msg_a20:     db '[Stage 2] A20 OK',13,10,0
msg_loading: db '[Stage 2] Loading kernel...',13,10,0
msg_loaded:  db '[Stage 2] Kernel loaded',13,10,0
msg_gdt:     db '[Stage 2] GDT OK',13,10,0
msg_pmode:   db '[Stage 2] Entering PM...',13,10,0
msg_error:   db '[Stage 2] DISK ERROR!',13,10,0

; ============================================================================
; 32-bit Protected Mode
; ============================================================================
[BITS 32]
pmode_entry:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Debug: Print "P" to show we're in protected mode
    mov byte [0xB8000], 'P'
    mov byte [0xB8001], 0x0A  ; Light green

    ; Debug: Print "M" 
    mov byte [0xB8002], 'M'
    mov byte [0xB8003], 0x0A

    ; Copy kernel from 0x20000 to 0x100000
    mov esi, KERNEL_TEMP_ADDR
    mov edi, KERNEL_LOAD_ADDR
    mov ecx, (KERNEL_SECTORS * 512) / 4
    cld
    rep movsd

    ; Debug: Print "C" to show copy done
    mov byte [0xB8004], 'C'
    mov byte [0xB8005], 0x0A

    ; Debug: show first byte at temp location (should be 0xC6)
    mov al, [KERNEL_TEMP_ADDR]
    add al, 0x30
    mov byte [0xB8006], al
    mov byte [0xB8007], 0x0E

    ; Debug: show first byte at kernel location (should also be 0xC6) 
    mov al, [KERNEL_LOAD_ADDR]
    add al, 0x30
    mov byte [0xB8008], al
    mov byte [0xB8009], 0x0E

    ; Debug: Print "J" before jump
    mov byte [0xB800A], 'J'
    mov byte [0xB800B], 0x0A

    ; Jump to kernel using indirect jump (fixes ORG 0 relative addressing issue)
    mov eax, KERNEL_LOAD_ADDR
    jmp eax

.hang:
    hlt
    jmp .hang

; ============================================================================
times 8192 - ($ - $$) db 0

; ============================================================================
; KontolOS Kernel Entry Point
; ============================================================================

[BITS 32]

global _start
extern kernel_main

section .text

_start:
    ; DEBUG: Write 'K' to screen immediately to prove we're here
    mov byte [0xB8008], 'K'
    mov byte [0xB8009], 0x0E    ; Yellow

    ; Set up the stack - use a fixed address for now
    mov esp, 0x90000

    ; DEBUG: Write 'S' to show stack is set
    mov byte [0xB800A], 'S'
    mov byte [0xB800B], 0x0E

    ; Clear direction flag
    cld

    ; DEBUG: Write 'C' before calling kernel_main
    mov byte [0xB800C], 'C'
    mov byte [0xB800D], 0x0E

    ; Call the C kernel main function
    call kernel_main

    ; DEBUG: Write 'R' if kernel_main returns
    mov byte [0xB800E], 'R'
    mov byte [0xB800F], 0x0E

    ; If kernel_main returns, halt the system
.hang:
    cli
    hlt
    jmp .hang

; Stack is set up by bootloader, we use 0x90000

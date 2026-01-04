; ============================================================================
; KontolOS Interrupt Service Routines
; ============================================================================

[BITS 32]

; Export ISR and IRQ handlers
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
global irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15

global idt_load

; Import C handlers
extern isr_handler
extern irq_handler

; ============================================================================
; Load IDT
; ============================================================================
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; ============================================================================
; ISR Common Stub
; ============================================================================
isr_common_stub:
    ; Save all registers
    pusha

    ; Save data segment
    mov ax, ds
    push eax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp
    call isr_handler
    add esp, 4

    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore registers
    popa

    ; Clean up error code and ISR number
    add esp, 8

    ; Return from interrupt
    iret

; ============================================================================
; IRQ Common Stub
; ============================================================================
irq_common_stub:
    ; Save all registers
    pusha

    ; Save data segment
    mov ax, ds
    push eax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp
    call irq_handler
    add esp, 4

    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore registers
    popa

    ; Clean up error code and IRQ number
    add esp, 8

    ; Return from interrupt
    iret

; ============================================================================
; Exception ISRs (0-31)
; Some exceptions push an error code, some don't
; ============================================================================

; ISR 0: Division By Zero
isr0:
    push dword 0        ; Dummy error code
    push dword 0        ; Interrupt number
    jmp isr_common_stub

; ISR 1: Debug
isr1:
    push dword 0
    push dword 1
    jmp isr_common_stub

; ISR 2: Non Maskable Interrupt
isr2:
    push dword 0
    push dword 2
    jmp isr_common_stub

; ISR 3: Breakpoint
isr3:
    push dword 0
    push dword 3
    jmp isr_common_stub

; ISR 4: Overflow
isr4:
    push dword 0
    push dword 4
    jmp isr_common_stub

; ISR 5: Bound Range Exceeded
isr5:
    push dword 0
    push dword 5
    jmp isr_common_stub

; ISR 6: Invalid Opcode
isr6:
    push dword 0
    push dword 6
    jmp isr_common_stub

; ISR 7: Device Not Available
isr7:
    push dword 0
    push dword 7
    jmp isr_common_stub

; ISR 8: Double Fault (has error code)
isr8:
    push dword 8
    jmp isr_common_stub

; ISR 9: Coprocessor Segment Overrun
isr9:
    push dword 0
    push dword 9
    jmp isr_common_stub

; ISR 10: Invalid TSS (has error code)
isr10:
    push dword 10
    jmp isr_common_stub

; ISR 11: Segment Not Present (has error code)
isr11:
    push dword 11
    jmp isr_common_stub

; ISR 12: Stack-Segment Fault (has error code)
isr12:
    push dword 12
    jmp isr_common_stub

; ISR 13: General Protection Fault (has error code)
isr13:
    push dword 13
    jmp isr_common_stub

; ISR 14: Page Fault (has error code)
isr14:
    push dword 14
    jmp isr_common_stub

; ISR 15: Reserved
isr15:
    push dword 0
    push dword 15
    jmp isr_common_stub

; ISR 16: x87 Floating-Point Exception
isr16:
    push dword 0
    push dword 16
    jmp isr_common_stub

; ISR 17: Alignment Check (has error code)
isr17:
    push dword 17
    jmp isr_common_stub

; ISR 18: Machine Check
isr18:
    push dword 0
    push dword 18
    jmp isr_common_stub

; ISR 19: SIMD Floating-Point Exception
isr19:
    push dword 0
    push dword 19
    jmp isr_common_stub

; ISR 20: Virtualization Exception
isr20:
    push dword 0
    push dword 20
    jmp isr_common_stub

; ISR 21: Control Protection Exception (has error code)
isr21:
    push dword 21
    jmp isr_common_stub

; ISR 22-27: Reserved
isr22:
    push dword 0
    push dword 22
    jmp isr_common_stub

isr23:
    push dword 0
    push dword 23
    jmp isr_common_stub

isr24:
    push dword 0
    push dword 24
    jmp isr_common_stub

isr25:
    push dword 0
    push dword 25
    jmp isr_common_stub

isr26:
    push dword 0
    push dword 26
    jmp isr_common_stub

isr27:
    push dword 0
    push dword 27
    jmp isr_common_stub

; ISR 28: Hypervisor Injection Exception
isr28:
    push dword 0
    push dword 28
    jmp isr_common_stub

; ISR 29: VMM Communication Exception (has error code)
isr29:
    push dword 29
    jmp isr_common_stub

; ISR 30: Security Exception (has error code)
isr30:
    push dword 30
    jmp isr_common_stub

; ISR 31: Reserved
isr31:
    push dword 0
    push dword 31
    jmp isr_common_stub

; ============================================================================
; IRQ handlers (0-15) - mapped to interrupts 32-47
; ============================================================================

irq0:
    push dword 0
    push dword 32
    jmp irq_common_stub

irq1:
    push dword 0
    push dword 33
    jmp irq_common_stub

irq2:
    push dword 0
    push dword 34
    jmp irq_common_stub

irq3:
    push dword 0
    push dword 35
    jmp irq_common_stub

irq4:
    push dword 0
    push dword 36
    jmp irq_common_stub

irq5:
    push dword 0
    push dword 37
    jmp irq_common_stub

irq6:
    push dword 0
    push dword 38
    jmp irq_common_stub

irq7:
    push dword 0
    push dword 39
    jmp irq_common_stub

irq8:
    push dword 0
    push dword 40
    jmp irq_common_stub

irq9:
    push dword 0
    push dword 41
    jmp irq_common_stub

irq10:
    push dword 0
    push dword 42
    jmp irq_common_stub

irq11:
    push dword 0
    push dword 43
    jmp irq_common_stub

irq12:
    push dword 0
    push dword 44
    jmp irq_common_stub

irq13:
    push dword 0
    push dword 45
    jmp irq_common_stub

irq14:
    push dword 0
    push dword 46
    jmp irq_common_stub

irq15:
    push dword 0
    push dword 47
    jmp irq_common_stub

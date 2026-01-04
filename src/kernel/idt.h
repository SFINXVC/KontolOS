/*
 * KontolOS Interrupt Descriptor Table Header
 */

#ifndef IDT_H
#define IDT_H

#include "../include/types.h"

/* Number of IDT entries */
#define IDT_ENTRIES 256

/* IDT entry flags */
#define IDT_FLAGS_INTERRUPT     0x8E    /* Present, Ring 0, 32-bit interrupt gate */
#define IDT_FLAGS_TRAP          0x8F    /* Present, Ring 0, 32-bit trap gate */
#define IDT_FLAGS_USER          0xEE    /* Present, Ring 3, 32-bit interrupt gate */

/* PIC ports */
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

/* PIC commands */
#define PIC_EOI         0x20    /* End of interrupt */
#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01

/* IDT entry structure */
struct idt_entry {
    uint16_t base_low;      /* Lower 16 bits of handler address */
    uint16_t sel;           /* Kernel segment selector */
    uint8_t always0;        /* Must be zero */
    uint8_t flags;          /* Type and attributes */
    uint16_t base_high;     /* Upper 16 bits of handler address */
} __attribute__((packed));

/* IDT pointer structure */
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Interrupt frame (pushed by CPU and our ISR stub) */
struct interrupt_frame {
    /* Pushed by our ISR stub */
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;

    /* Pushed by CPU */
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));

/* ISR handler function type */
typedef void (*isr_handler_t)(struct interrupt_frame *frame);

/* Function declarations */
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void irq_register_handler(uint8_t irq, isr_handler_t handler);
void irq_unregister_handler(uint8_t irq);

/* Assembly function to load IDT */
extern void idt_load(uint32_t idt_ptr);

/* Handlers called from assembly */
void isr_handler(struct interrupt_frame *frame);
void irq_handler(struct interrupt_frame *frame);

#endif /* IDT_H */

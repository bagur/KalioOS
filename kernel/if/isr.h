/* KalioOS (C) 2020 Pranav Bagur */

/*
 * https://web.archive.org/web/20160327011227/
 * http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
 *
 * There are times when you want to interrupt the processor. You want to stop it
 * doing what it is doing, and force it to do something different. An example of
 * this is when an timer or keyboard interrupt request (IRQ) fires. An interrupt
 * is like a POSIX signal - it tells you that something of interest has
 * happened. The processor can register 'signal handlers' (interrupt handlers)
 * that deal with the interrupt, then return to the code that was running before
 * it fired. Interrupts can be fired externally, via IRQs, or internally, via
 * the 'int n' instruction. There are very useful reasons for wanting to do fire
 * interrupts from software, but that's for another chapter!
 *
 * The Interrupt Descriptor Table tells the processor where to find handlers for
 * each interrupt. It is very similar to the GDT. It is just an array of
 * entries, each one corresponding to an interrupt number. There are 256
 * possible interrupt numbers, so 256 must be defined. If an interrupt occurs
 * and there is no entry for it (even a NULL entry is fine), the processor will
 * panic and reset.
 *
 * PART 1 (IRQs 0 - 31)
 * --------------------
 * The processor will sometimes need to signal your kernel. Something major may
 * have happened, such as a divide-by-zero, or a page fault. To do this, it uses
 * the first 32 interrupts. It is therefore doubly important that all of these are
 * mapped and non-NULL - else the CPU will triple-fault and reset
 *
 * PART 2 (IRQs 32 - 47)
 * ---------------------
 * All devices that are interrupt-capable have a line connecting them to the PIC
 * (programmable interrupt controller). The PIC is the only device that is directly
 * connected to the CPU's interrupt pin. It is used as a multiplexer, and has the
 * ability to prioritise between interrupting devices. It is, essentially, a
 * glorified 8-1 multiplexer. At some point, someone somewhere realised that 8 IRQ
 * lines just wasn't enough, and they daisy-chained another 8-1 PIC beside the
 * original. So in all modern PCs, you have 2 PICs, the master and the slave,
 * serving a total of 15 interruptable devices (one line is used to signal the
 * slave PIC).
 * 
 * The other clever thing about the PIC is that you can change the interrupt number
 * it delivers for each IRQ line. This is referred to as remapping the PIC and is
 * actually extremely useful. When the computer boots, the default interrupt
 * mappings are:
 * 
 *     IRQ 0..7 - INT 0x8..0xF IRQ 8..15 - INT 0x70..0x77
 * 
 * This causes us somewhat of a problem. The master's IRQ mappings (0x8-0xF)
 * conflict with the interrupt numbers used by the CPU to signal exceptions and
 * faults (see PART 1). The normal thing to do is to remap the PICs so that
 * IRQs 0..15 correspond to ISRs 32..47 (31 being the last CPU-used ISR). 
 */

#ifndef __IDT_H
#define __IDT_H

#include "../../common/if/types.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

/* STRUCT idt_entry_t - Describes an entry in IDT */
typedef struct __attribute__((packed)) _entry_idt
{
  ub2 base_lo_entry_idt;  // The lower 16 bits of handler address
  ub2 sel_entry_idt;      // Kernel segment selector.
  ub1 always0_entry_idt;  // This must always be zero.
  ub1 flags_entry_idt;    // More flags. See documentation.
  ub2 base_hi_entry_idt;  // The upper 16 bits of handler address
} idt_entry_t;

/* STRUCT idt_t - Stores base and limit of our IDT */
typedef struct __attribute__((packed)) _idt
{
  ub2 limit_idt;          // table limit
  ub4 base_idt;           // bp of the table
} idt_t;

/* STRUCT registers_t - describes the stack for isr_handler */
/* 
 * 1. When an interrupt fires, the processor automatically pushes
 *    information about the processor state onto the stack. 
 *    This includes the code segment, instruction pointer, 
 *    flags register, stack segment and stack pointer
 * 2. We push the intr number and the error code (see isr0 - 31)
 * 3. pusha in isr_common pushes eax, ecx, edx, ebx, esp, ebp, esi, edi
 * 4. push eax from isr_common
 */
typedef struct {
   ub4 ds;                                     /* 4 above */
   ub4 edi, esi, ebp, esp, ebx, edx, ecx, eax; /* 3 above */
   ub4 int_no, err_code;                       /* 2 above */
   ub4 eip, cs, eflags, useresp, ss;           /* 1 above */
} registers_t;

#define KERN_CS 0x08
#define IDT_ENTRIES 256
idt_entry_t idt[IDT_ENTRIES];
idt_t idt_desc;

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#define MASTER_PIC_CMD  0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC_CMD   0xA0
#define SLAVE_PIC_DATA  0xA1

/* How an IRQ handler must look */
typedef void (*isr_t)(registers_t); 
isr_t irq_handlers[256];

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 

/* interrupt handlers */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* common handler for CPU faults and exceptions */
void isr_handler(registers_t regs);

/* common handler for device IRQs */
void irq_handler(registers_t regs);

/* register handler for device IRQs */
void register_handler(ub4 irq_num, isr_t handler);

/* isr init function   */
bool isr_init_func(void);

/* isr exit function   */
void isr_exit_func(void);

#endif

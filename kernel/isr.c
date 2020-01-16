/* KalioOS (C) 2020 Pranav Bagur */

#include "if/isr.h"
#include "../drivers/if/screen.h"
#include "../drivers/if/port.h"
#include "../common/if/common.h"

/* -------------------------------------------------------------------------- 
                         Static function declarations
   -------------------------------------------------------------------------- */ 

static void init_idt_entry(ub4 idx, ub4 handler);

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

ub1 *intr_to_str[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* -------------------------------------------------------------------------- 
                         Static functions
   -------------------------------------------------------------------------- */ 

/* 
 * SF: init_idt_entry - initialize an IDT entry
 * 
 * ARGS :-
 *   idx     - IDT table entry index
 *   handler - address of the handler
 *
 * RET
 */
static void
init_idt_entry(ub4 idx, ub4 handler)
{
  idt[idx].base_lo_entry_idt = handler & 0xFFFF;
  idt[idx].base_hi_entry_idt = (handler >> 16) & 0xFFFF;
  idt[idx].sel_entry_idt     = KERN_CS;
  idt[idx].always0_entry_idt = 0;
  idt[idx].flags_entry_idt   = 0x8E;
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 

/* 
 * EF: isr_handler - common handler for CPU faults and exceptions (IRQs 0 - 31)
 * 
 * ARGS :-
 *   regs - see isr.h
 *
 * RET
 */
void 
isr_handler(registers_t regs)
{
  ub4 irq_num = regs.int_no;

  if (irq_handlers[irq_num] != 0) {
    /* call the registered handler */
    isr_t handler = irq_handlers[irq_num];
    handler(regs);
  }
  else {
    printk("recieved interrupt: ");
    printk(intr_to_str[regs.int_no]);
    printk("\n");
    PANIC("CPU FAULT");
  }
}

/* 
 * EF: irq_handler - common handler for device IRQs (IRQs 32 - 47)
 * 
 * ARGS :-
 *   regs - see isr.h
 *
 * RET
 */
void 
irq_handler(registers_t regs)
{
  ub4 irq_num = regs.int_no;

  ASSERT((irq_num >= IRQ0) && (irq_num <= IRQ15));

  /* 
   * After every interrupt we need to send an EOI to the PICs
   * or they will not send another interrupt again 
   */
  if (irq_num >= IRQ8) {
    // Send reset signal to slave if this interrupt involved the slave
    port_byte_out(SLAVE_PIC_CMD, 0x20);
  }
  
  // Send reset signal to master
  port_byte_out(MASTER_PIC_CMD, 0x20);

  if (irq_handlers[irq_num] != 0) {
    /* call the registered handler */
    isr_t handler = irq_handlers[irq_num];
    handler(regs);
  }
}

/* 
 * EF: register_handler - register handler for faults/device IRQs
 * 
 * ARGS :-
 *   irq_num - IRQ #
 *   handler - func ptr
 *
 * RET
 */
void 
register_handler(ub4 irq_num, isr_t handler)
{
  irq_handlers[irq_num] = handler; 
}

/* 
 * EF: isr_init_func - init ISR
 * 
 * ARGS :-
 *
 * RET - TRUE
 */
bool
isr_init_func()
{
  ub4 i = 0;

  /* Initialize IDT */
  init_idt_entry(0,  (ub4)isr0);
  init_idt_entry(1,  (ub4)isr1);
  init_idt_entry(2,  (ub4)isr2);
  init_idt_entry(3,  (ub4)isr3);
  init_idt_entry(4,  (ub4)isr4);
  init_idt_entry(5,  (ub4)isr5);
  init_idt_entry(6,  (ub4)isr6);
  init_idt_entry(7,  (ub4)isr7);
  init_idt_entry(8,  (ub4)isr8);
  init_idt_entry(9,  (ub4)isr9);
  init_idt_entry(10, (ub4)isr10);
  init_idt_entry(11, (ub4)isr11);
  init_idt_entry(12, (ub4)isr12);
  init_idt_entry(13, (ub4)isr13);
  init_idt_entry(14, (ub4)isr14);
  init_idt_entry(15, (ub4)isr15);
  init_idt_entry(16, (ub4)isr16);
  init_idt_entry(17, (ub4)isr17);
  init_idt_entry(18, (ub4)isr18);
  init_idt_entry(19, (ub4)isr19);
  init_idt_entry(20, (ub4)isr20);
  init_idt_entry(21, (ub4)isr21);
  init_idt_entry(22, (ub4)isr22);
  init_idt_entry(23, (ub4)isr23);
  init_idt_entry(24, (ub4)isr24);
  init_idt_entry(25, (ub4)isr25);
  init_idt_entry(26, (ub4)isr26);
  init_idt_entry(27, (ub4)isr27);
  init_idt_entry(28, (ub4)isr28);
  init_idt_entry(29, (ub4)isr29);
  init_idt_entry(30, (ub4)isr30);
  init_idt_entry(31, (ub4)isr31);


  // Remap the PIC
  port_byte_out(MASTER_PIC_CMD,  0x11);
  port_byte_out(SLAVE_PIC_CMD,   0x11);
  port_byte_out(MASTER_PIC_DATA, 0x20);
  port_byte_out(SLAVE_PIC_DATA,  0x28);
  port_byte_out(MASTER_PIC_DATA, 0x04);
  port_byte_out(SLAVE_PIC_DATA,  0x02);
  port_byte_out(MASTER_PIC_DATA, 0x01);
  port_byte_out(SLAVE_PIC_DATA,  0x01);
  port_byte_out(MASTER_PIC_DATA, 0x0);
  port_byte_out(SLAVE_PIC_DATA,  0x0); 

  // Install the IRQs
  init_idt_entry(32, (ub4)irq0);
  init_idt_entry(33, (ub4)irq1);
  init_idt_entry(34, (ub4)irq2);
  init_idt_entry(35, (ub4)irq3);
  init_idt_entry(36, (ub4)irq4);
  init_idt_entry(37, (ub4)irq5);
  init_idt_entry(38, (ub4)irq6);
  init_idt_entry(39, (ub4)irq7);
  init_idt_entry(40, (ub4)irq8);
  init_idt_entry(41, (ub4)irq9);
  init_idt_entry(42, (ub4)irq10);
  init_idt_entry(43, (ub4)irq11);
  init_idt_entry(44, (ub4)irq12);
  init_idt_entry(45, (ub4)irq13);
  init_idt_entry(46, (ub4)irq14);
  init_idt_entry(47, (ub4)irq15);
    
  /* Initialize IDT descriptor */
  idt_desc.base_idt  = (ub4) &idt;
  idt_desc.limit_idt = IDT_ENTRIES * sizeof(idt_entry_t) - 1;
  
  printk_system("Initialized IDT entries..");

  /* Load IDT descriptor */
  __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_desc));
  
  printk_system("Finished loading IDT..");
  return true;
}

/* 
 * EF: isr_exit_func - exit ISR
 * 
 * ARGS :-
 *
 * RET -
 */
void
isr_exit_func()
{
}

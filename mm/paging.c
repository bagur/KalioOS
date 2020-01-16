/* KalioOS (C) 2020 Pranav Bagur */

#include "if/paging.h"
#include "if/memory.h"
#include "../kernel/if/isr.h"
#include "../drivers/if/screen.h"
#include "../common/if/common.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
page_dir_t *cur_dir;

/* -------------------------------------------------------------------------- 
                         Inline functions
   -------------------------------------------------------------------------- */ 
/* === SIF: Set frame address of a PTE === */
static inline void 
set_frame_addr(page_entry_t *pte, ub4 addr)
{
  ub4 pte_new;

  /* Don't touch the lower 12 bits */
  pte_new = (*pte & 0xFFF);

  /* Set the addr in the higher 20 bits */
  pte_new = (pte_new | (addr & 0xFFFFF000));
  *pte = pte_new;
}

/* === SIF: Get frame address of a PTE === */
static inline ub4
get_frame_addr(page_entry_t *pte)
{
  return (*pte & 0xFFFFF000);
}

/* === SIF: Set bit of a PTE === */
static inline void 
set_pte_bit(page_entry_t *pte, ub4 offset)
{
  *pte = (*pte | (1ULL << offset));
}

/* === SIF: Get present bit of a PTE === */
static inline ub4
get_pte_bit(page_entry_t *pte, ub4 offset)
{
  return (*pte & (1ULL << offset));
}

/* === SIF: Get present bit of a PTE === */
static inline ub4
clr_pte_bit(page_entry_t *pte, ub4 offset)
{
  *pte = (*pte & ~(1ULL << offset));
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: page_fault_handler - Page fault handler
 * 
 * ARGS :-
 *   registers_t (see irq.h)
 *
 * RET
 */
void 
page_fault_handler(registers_t regs)
{
  ub4 addr;
  
  /* Get the faulting address */
  asm volatile("mov %%cr2, %0" : "=r" (addr));

  printk("Page fault at: ");
  printk_num(addr);
  printk("\n");

  if (!(regs.err_code & 0x1))
    printk("page not present\n");

  if (regs.err_code & 0x2)
    printk("page read-only\n");

  if (regs.err_code & 0x4)
    printk("processor was in user mode\n");

  if (regs.err_code & 0x8)
    printk("CPU reserved bits corrupted\n");
  
  PANIC("Page fault");
}

/* 
 * EF: switch_page_dir - Switch page directory
 * 
 * ARGS :-
 *   dir - directory to switch to
 *
 * RET
 */
void switch_page_dir(page_dir_t *dir)
{
   ub4 cr0;

   asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}

/* 
 * EF: kmalloc - Reserve memory and add page entry in cur_dir 
 * 
 * ARGS :-
 *   sz - size in bytes to reserve
 *
 * RET
 *   starting phys addr of the reserved block
 */
ub4
kmalloc(ub4 sz)
{
  /* kmalloc_mem reserves a contigious block and returns the phys addr */
  ub4 phys_addr, cur_addr;

  phys_addr = kmalloc_mem(sz, true);
  
  cur_addr = phys_addr;
  while (cur_addr < (phys_addr + sz))
  {
    add_page_table_entry(cur_addr, cur_addr, cur_dir);
    cur_addr += PAGE_SIZE;
  }

  memset((ub1 *) phys_addr, sz, 0);
  return phys_addr;
}

/* 
 * EF: add_page_table_entry - Add page table entry
 * 
 * ARGS :-
 *   address - virtual address
 *   dir     - page directory
 *
 * RET
 */
void 
add_page_table_entry(ub4 virt_addr, ub4 phys_addr, page_dir_t *dir)
{
  /* Find out which page table the address belongs to */
  page_table_t *pt;
  page_entry_t *pte;
  ub4           first_idx  = (virt_addr >> PAGE_DIR_OFFSET) & PAGE_DIR_MASK;
  ub4           second_idx = (virt_addr >> PAGE_TABLE_OFFSET) & PAGE_TABLE_MASK;

  /* If first level entry does not exist add it */
  if(!dir->page_tables[first_idx])
  {
    ub4 i  = 0;
    ub4 sz = sizeof(page_table_t);

    ASSERT(sz == PAGE_SIZE);
    pt = (page_table_t *)kmalloc_mem(sz, true);
    //memset((ub1 *)pt, sz, 0);

    dir->page_tables[first_idx] = pt;
    dir->tablesPhysical[first_idx] = ((ub4)pt | 0x3);
  }

  /* Make sure there is no page table entry already */
  pt = dir->page_tables[first_idx];
  ASSERT(pt->page_entries[second_idx] == 2);
  pt->page_entries[second_idx] = phys_addr | 0x3;
  //pte = &pt->page_entries[second_idx];
  //set_frame_addr(pte, phys_addr);
  //set_pte_bit(pte, PRESENT_OFFSET);
  //set_pte_bit(pte, RW_OFFSET);
}

/* 
 * EF: paging_init_func - module init function
 * 
 * ARGS :-
 *
 * RET - TRUE
 */
bool
paging_init_func()
{
  ub4 cur_addr = 0;
  int idx      = 0;
  ub4 sz       = sizeof(page_dir_t);

  cur_dir = (page_dir_t *)kmalloc_mem(sz, true);
  memset((ub1 *)cur_dir, sz, 0);
  
  while (cur_addr < get_free_mem_ptr())
  {
    add_page_table_entry(cur_addr, cur_addr, cur_dir);
    cur_addr += PAGE_SIZE;
  }
  
  register_handler(14, page_fault_handler);
  printk_system("Initialized paging..");
  switch_page_dir(cur_dir);
}

/* 
 * EF: paging_exit_func - module exit function
 * 
 * ARGS :-
 *
 * RET 
 */
void
paging_exit_func()
{
}

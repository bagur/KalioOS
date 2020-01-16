/* KalioOS (C) 2020 Pranav Bagur */
/* 
 * Please watch this series
 * https://www.youtube.com/watch?v=qcBIvnQt0Bw
 *
 * http://www.osdever.net/tutorials/view/implementing-basic-paging
 */
#ifndef __PAGING_H
#define __PAGING_H

#include "../../common/if/types.h"
#include "../../kernel/if/isr.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
/* 
 * Describes a page table entry
 * +------------------+----------+--------+----+----+--------+----+------+-----+
 * |frame addr (31-12)| AVL(11-9)|RSV(8-7)|D(6)|A(5)|RSV(4-3)|U(2)|R/W(1)| P(0)|
 * +------------------+----------+--------+----+----+--------+----+------+-----+
 *
 * P    - Set if the page is present in memory.
 * R/W  - If set, that page is writeable (user mode only)
 * U    - If set, this is a user-mode page
 * RSV  - These are used by the CPU internally and cannot be trampled.
 * A    - Set if the page has been accessed (Gets set by the CPU).
 * D    - Set if the page has been written to (dirty).
 * AVL  - These 3 bits are unused and available for kernel-use.
 * ADDR - The high 20 bits of the frame address in physical memory
 *
 */
#define PRESENT_OFFSET  0
#define RW_OFFSET       1
#define USERMODE_OFFSET 2

#define PAGE_DIR_OFFSET (22ULL)
#define PAGE_DIR_LEN    (1024ULL)
#define PAGE_DIR_MASK   ((1ULL << PAGE_DIR_LEN) - 1ULL)


#define PAGE_TABLE_OFFSET (12ULL)
#define PAGE_TABLE_LEN    (1024ULL)
#define PAGE_TABLE_MASK   ((1ULL << PAGE_TABLE_LEN) - 1ULL)

typedef ub4 page_entry_t;

/* STRUCT page_table_t - Describes a page table */
typedef struct _page_table
{
  page_entry_t page_entries[1024];
} page_table_t;

/* STRUCT page_dir_t - Describes a page dir */
typedef struct _page_dir
{
  page_table_t *page_tables[1024];
  ub4 tablesPhysical[1024];
} page_dir_t;

extern page_dir_t *cur_dir;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* page fault handler */
void page_fault_handler(registers_t regs);

/* switch page directory */
void switch_page_dir(page_dir_t *dir);

/* add page table entry */
void add_page_table_entry(ub4 virt_addr, ub4 phys_addr, page_dir_t *dir);

/* reserve frame and add a page table entry */
ub4 kmalloc(ub4 size);

/* module init function   */ 
bool paging_init_func(void);

/* module exit function   */ 
void paging_exit_func(void);


#endif

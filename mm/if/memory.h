/* KalioOS (C) 2020 Pranav Bagur */

/* 
 * The routines defined here can be used by the rest of the kernel to reserve
 * memory. The assumption we make here is that once reserved, memory cannot
 * be freed. Thus, we run out of memory once we reach the end and panic.
 *
 * The heap will reserve a chunk of memory using these routines and manage it
 */
#ifndef __MEMORY_H
#define __MEMORY_H

#include "../../common/if/types.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
/* 
 * TODO: FREE_MEM_START hardcoded for now. The offset needs to come
 * from the linker
 */
#define FREE_MEM_START 0x100000
#define MEM_SIZE       0x1000000 /* 16 MB */

#define PAGE_SIZE      0x1000    /* 4 KB */

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 

/* Return free mem ptr  */
ub4 get_free_mem_ptr(void);

/* Reserve memory chunk  */
ub4 kmalloc_mem(ub4 size, bool align);

/* Set all the bytes in a mem range to val */
void memset(ub1 *addr, ub4 len, ub1 val);

/* copy 'len' bytes from source to dest */
void memcpy(ub1 *src, ub1 *dest, ub4 len);

#endif

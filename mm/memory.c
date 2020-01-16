/* KalioOS (C) 2020 Pranav Bagur */

#include "if/memory.h"
#include "../common/if/common.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
ub4 free_mem_ptr = FREE_MEM_START;

/* -------------------------------------------------------------------------- 
                         Inline functions
   -------------------------------------------------------------------------- */ 
/* === SIF: return address aligned on page boundary === */
ub4 
page_align(ub4 addr)
{
  return (addr & 0xFFFFF000);
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 

/* 
 * EF: get_free_mem_ptr - return where the free mem ptr is
 * 
 * ARGS :-
 *
 * RET -
 *   free_mem_ptr
 */
ub4
get_free_mem_ptr()
{
  return free_mem_ptr;
}

/* 
 * EF: kmalloc_mem - Reserve memory chunk
 * 
 * ARGS :-
 *   size  - size to be reserved
 *   align - should be aligned on page boundary 
 *
 * RET -
 *   reserved addr
 */
ub4 
kmalloc_mem(ub4 size, bool align)
{
  ub4 cur_mem_ptr;

  if (align)
    free_mem_ptr = page_align(free_mem_ptr + PAGE_SIZE - 1);

  if (free_mem_ptr >= (MEM_SIZE))
    PANIC("No memory");

  cur_mem_ptr = free_mem_ptr;
  free_mem_ptr += size;

  return cur_mem_ptr;
}

/* 
 * EF: memset - set all the bytes in a mem range to val
 * 
 * ARGS :-
 *   addr - start address
 *   len  - length
 *   val  - val to set to
 *
 * RET -
 */
void 
memset(ub1 *addr, ub4 len, ub1 val)
{
  ub4 i = 0;
  ub1 *temp = (ub1 *)addr;

  for (i = 0; i < len; i++)
    temp[i] = val;
}

/* 
 * EF: memcpy - copy 'len' bytes from source to dest
 * 
 * ARGS :-
 *   src  - source address
 *   dest - destination address 
 *   len  - length
 *
 * RET -
 */
void 
memcpy(ub1 *src, ub1 *dest, ub4 len)
{
  ub4 i = 0;

  for (i = 0; i < len; i++)
    dest[i] = src[i];
}

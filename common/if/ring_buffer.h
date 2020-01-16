/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "types.h"
#include "../../mm/if/memory.h"
#include "../../mm/if/heap.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
typedef struct _ring_buf
{
  ub4   size;
  void *buf;
  ub4   capacity;
  ub4   count;
  ub4   head;
  ub4   tail;
} ring_buf;

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 

/* Get capacity of the ring buffer */
ub4       rb_get_capacity(ring_buf *rb);

/* Get num slots available in the ring buffer */
ub4       rb_get_avail(ring_buf *rb);

/* Push item into ring buffer */
bool      rb_push(ring_buf *rb, void *item);

/* Pop item from ring buffer */
bool      rb_pop(ring_buf *rb, void *item);

/* Initialize ring buffer */
ring_buf *rb_init(ub4 size, ub4 capacity);

/* Free ring buffer */
void      rb_free(ring_buf *rb);


#endif

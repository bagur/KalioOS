/* KalioOS (C) 2020 Pranav Bagur */

#include "if/ring_buffer.h"

/* -------------------------------------------------------------------------- 
                         Static inline functions
   -------------------------------------------------------------------------- */ 
/* === SIF: Is ring buffer full === */
static inline bool
rb_full(ring_buf *rb)
{
  return (rb_get_avail(rb) == 0);
}

/* === SIF: Is ring buffer empty === */
static inline bool
rb_empty(ring_buf *rb)
{
  return (rb->count == 0);
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: rb_get_capacity - Get capacity of the ring buffer
 * 
 * ARGS :-
 *   rb - address of ring buf
 *
 * RET
 *   ub4 - capacity of the ring buffer
 */
ub4
rb_get_capacity(ring_buf *rb)
{
  return rb->capacity;
}

/* 
 * EF: rb_get_avail - Get available slots in the ring buffer
 * 
 * ARGS :-
 *   rb - address of ring buf
 *
 * RET
 *   ub4 - space left in the ring buffer
 */
ub4
rb_get_avail(ring_buf *rb)
{
  return (rb->capacity - rb->count);
}

/* 
 * EF: rb_push - Push item into ring buf
 * 
 * ARGS :-
 *   rb   - address of ring buf
 *   item - address of item to push
 *
 * RET
 *   true iff successful
 *   false otherwise
 */
bool
rb_push(ring_buf *rb, void *item)
{
  //printk_char('0');
  if (rb_full(rb))
    return false;

  //printk_char('1');
  /* TODO Locking */
  memcpy((ub1 *)item, (ub1 *)(rb->buf + rb->tail * rb->size), rb->size);
  //printk_char('2');
  rb->tail++;
  rb->count++;
  if (rb->tail == rb->capacity)
    rb->tail = 0;

  //printk_char('3');
  return true;
}

/* 
 * EF: rb_pop - Pop an item from ring buf
 * 
 * ARGS :-
 *   rb   - address of ring buf
 *   item - address of item to fill
 *
 * RET
 *   true iff successful
 *   false otherwise
 */
bool
rb_pop(ring_buf *rb, void *item)
{
  if (rb_empty(rb))
    return false;

  /* TODO Locking */
  memcpy((ub1 *)(rb->buf + rb->head * rb->size), (ub1 *)item, rb->size);
  rb->head++;
  rb->count--;
  if (rb->head == rb->capacity)
    rb->head = 0;

  return true;
}

/* 
 * EF: rb_init - Initialize a new ring buffer
 * 
 * ARGS :-
 *   ub4 size     - size of each element in the buffer
 *   ub4 capacity - num of items the buffer should be able to hold
 *
 * RET
 *   address of new ring_buf
 */
ring_buf *
rb_init(ub4 size, ub4 capacity)
{
  ring_buf *rb;

  rb = (ring_buf *)kmalloc_heap(sizeof(*rb));
  if (!rb)
    return NULL;
  else {
    rb->buf      = (void *)kmalloc_heap(size * capacity);
    if (!rb->buf)
      goto err_exit;

    rb->size     = size;
    rb->capacity = capacity;
    rb->count    = 0;
    rb->head     = 0;
    rb->tail     = 0;
  }

  return rb;

err_exit:
  kfree_heap((ub4 *)rb);
  return NULL;
}

/* 
 * EF: rb_free - Free ring buffer
 * 
 * ARGS :-
 *   rb - address of ring buf
 *
 * RET
 */
void
rb_free(ring_buf *rb)
{
  kfree_heap((ub4 *)rb->buf);
  kfree_heap((ub4 *)rb);
}

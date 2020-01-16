/* KalioOS (C) 2020 Pranav Bagur */

#include "if/tests.h"
#include "../drivers/if/screen.h"
#include "../common/if/lock_intr.h"
#include "../common/if/ring_buffer.h"

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: test_cpu_fault - cpu fault
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_cpu_fault()
{
  asm volatile ("int $0x20");
  asm volatile ("int $0x4"); /* panic here */
  asm volatile ("int $0x5");
}

/* 
 * EF: test_page_fault - page allocation and fault
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_page_fault()
{
  ub4 *ptr = (ub4 *) kmalloc(0x1000);
  ub4 do_page_fault = *(ptr + 0x1);

  ub4 *ptr2 = (ub4 *)0xA0000000;
  do_page_fault = *(ptr2 + 0x1); /* page fault here */
}

/* 
 * EF: test_list - list add/remove and loop
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_list()
{
  ub4 count = 7;
  ub4 idx = 0;
  list *head = (list *)kmalloc(sizeof(*head));
  list *cur;

  list_init(head);
  for (idx = 0; idx < count; idx++) {
    list_test_st *test = (list_test_st *)kmalloc(sizeof(*test));
    convert_to_str(idx, test->val, 1);  
    test->val[1] = '\0';
    list_add_head(head, &test->link);
  }

  list_for_each(cur, head) {
    list_test_st *test = list_entry(cur, list_test_st, link);
    printk(test->val);
    printk(" ");
  }

  printk("\n");
  list *new_item = list_remove_front(head);
  list_test_st *test2 = list_entry(new_item, list_test_st, link);
  printk(test2->val);
  printk("\n");

  new_item = list_remove_front(head);
  test2 = list_entry(new_item, list_test_st, link);
  printk(test2->val);
  printk("\n");

  list_for_each(cur, head) {
    list_test_st *test = list_entry(cur, list_test_st, link);
    printk(test->val);
    printk(" ");
  }
}

/* 
 * EF: test_heap - heap malloc/free/grow
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_heap()
{
  ub4 idx = 0;
  ub4 max = 100;
  ub4 *addrs[max];

  for (idx = 0; idx < max; idx++) {
    ub4 *addr = (ub4 *)kmalloc_heap(20);
    kfree_heap(addr);
  }

  for (idx = 0; idx < max; idx++) {
    addrs[idx] = (ub4 *)kmalloc_heap(20);
  }

  for (idx = 0; idx < max; idx++) {
    kfree_heap(addrs[idx]);
  }
}

/* 
 * EF: timer_callback - timer callback routine
 * 
 * ARGS :-
 *
 * RET -
 */
void
timer_callback(ub8 data)
{
  printk("callback called with data: ");
  printk_num((ub4)data);
  printk("\n");
}

/* 
 * EF: test_timer - test delay code
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_timer()
{
  timer_t *timer = add_dyn_timer(60, &timer_callback, 5);
}

/* 
 * EF: test_lock - test locking code
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_lock()
{
  ub8 flags;
  ub8 var = 102134335;

  printk("looping\n");
  while(var--);
  var = 1021343355;
  printk("locking\n");
  lock_intr(&flags);
  while(var--);
  unlock_intr(&flags);
  printk("unlocked\n");
}

/* 
 * EF: test_ring_buf - test ring buffer
 * 
 * ARGS :-
 *
 * RET -
 */
void
test_ring_buf()
{
  ub4       capacity = 10;
  ub4       i        = 0;
  ub4       num      = 50;
  ring_buf *rb       = rb_init(sizeof(ub4), capacity);

  for (i = 0; i < capacity; i++) {
    rb_push(rb, &num);
    num++;
  }

  ASSERT(!rb_push(rb, &num));

  for (i = 0; i < capacity; i++) {
    if (rb_pop(rb, &num)) {
      printk_num(num);
      printk(" ");
    }
  }
  
  ASSERT(!rb_pop(rb, &num));
  ASSERT(!rb_pop(rb, &num));

  printk("\n");

  num = 59;
  rb_push(rb, &num);
  num++;
  rb_pop(rb, &num);
  rb_push(rb, &num);
  rb_push(rb, &num);
  num++;
  rb_push(rb, &num);
  rb_pop(rb, &num);
  num++;
  rb_push(rb, &num);
  num++;
  rb_push(rb, &num);
  num++;
  rb_push(rb, &num);
  rb_push(rb, &num);
  rb_pop(rb, &num);
  rb_push(rb, &num);
  num++;
  rb_push(rb, &num);
  num++;
  rb_push(rb, &num);
  rb_push(rb, &num);
  num++;
  rb_pop(rb, &num);
  num++;
  rb_push(rb, &num);

  while (rb_pop(rb, &num)) {
    printk_num(num);
    printk(" ");
  }
}

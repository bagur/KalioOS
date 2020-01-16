/* KalioOS (C) 2020 Pranav Bagur */

#include "if/timer.h"
#include "../kernel/if/isr.h"
#include "if/screen.h"
#include "../mm/if/heap.h"

ub8           ticks = 0;
timer_list_t *timer_glob;
ub4           list_delays[]  = {0, 50, 100, 500, 1000};
ub4           list_process[] = {1, 3, 8, 15, 50};

/* 
 * SF: add_dyn_timer_to_list - Add dynamic timer to the appropriate list
 * 
 * ARGS :-
 *   timer - address of dyn timer
 *
 * RET -
 */
static void
add_dyn_timer_to_list(timer_t *timer)
{
  ub4 i     = (N_TIMER_LISTS - 1);
  ub4 delay = timer->delay_timer;

  while (delay < (list_delays[i] + ticks)) i--;

  list_add_tail(&timer_glob->lists_timer[i], &timer->link_timer);
  timer_glob->lists_count_timer[i]++;
}

/* 
 * SF: free_dyn_deleted_timers - Free all dyn timers on del_list
 * 
 * ARGS :-
 *
 * RET -
 */
static void
free_dyn_deleted_timers()
{
  while (timer_glob->del_list_count_timer) {
    list    *item;
    timer_t *timer;

    item  = list_remove_front(&timer_glob->del_list_timer);
    timer_glob->del_list_count_timer--;

    timer = list_entry(item, timer_t, link_timer);
    kfree_heap((ub4 *)timer);
  }
}

/* 
 * SF: process_dyn_list - process all the dynamic timers on a list
 * 
 * ARGS :-
 *   cur_list - address of list
 *   idx      - index of list
 *
 * RET -
 */
static void
process_dyn_list(list *cur_list, ub4 idx)
{
  list tmp_list;
  ub4  tmp_list_count = 0;

  list_init(&tmp_list);
  /* 
   * Move everything to a local list
   * TODO implement list_for_each_safe and clean this up
   */
  while (timer_glob->lists_count_timer[idx]) {
    list    *item;
    timer_t *timer;

    item  = list_remove_front(&timer_glob->lists_timer[idx]);
    timer_glob->lists_count_timer[idx]--;

    timer = list_entry(item, timer_t, link_timer);
    list_add_tail(&tmp_list, &timer->link_timer);
    tmp_list_count++;
  }

  while (tmp_list_count) {
    list    *item;
    timer_t *timer;

    item  = list_remove_front(&tmp_list);
    tmp_list_count--;

    timer = list_entry(item, timer_t, link_timer);
    if (idx == 0 && ((ub4)timer->delay_timer <= ticks)) {
      timer->func_timer((ub8)timer->data_timer);
      list_add_tail(&timer_glob->del_list_timer, &timer->link_timer);
      timer_glob->del_list_count_timer++;
    }
    else {
      add_dyn_timer_to_list(timer);
    }
  }
}

/* 
 * SF: process_dyn_timers - process all the dynamic timers
 * 
 * ARGS :-
 *
 * RET -
 */
static void
process_dyn_timers()
{
  ub4 i;

  for (i = 0; i < N_TIMER_LISTS; i++) {
    if (((ub4)ticks % list_process[i]) == 0) { // ub8 __udivdi3 gcc err 
      process_dyn_list(&timer_glob->lists_timer[i], i);
    }
  }
}

/* 
 * SF: init_dyn_timer - Initialize dynamic timer module
 * 
 * ARGS :-
 *
 * RET -
 *   true if successful
 */
static bool
init_dyn_timer()
{
  ub4 i = 0;

  timer_glob = (timer_list_t *)kmalloc_heap(sizeof(*timer_glob));
  if (!timer_glob)
    return false;

  for (i = 0; i < N_TIMER_LISTS; i++) {
    list_init(&timer_glob->lists_timer[i]);
    timer_glob->lists_count_timer[i] = 0;
  }
  
  list_init(&timer_glob->del_list_timer);
  timer_glob->del_list_count_timer = 0;
  return true;
}

/* 
 * SF: teardown_dyn_timer - teardown dynamic timer module
 * 
 * ARGS :-
 *
 * RET -
 */
static void
teardown_dyn_timer()
{
  free_dyn_deleted_timers();
  kfree_heap((ub4 *)timer_glob);
}

/* 
 * EF: add_dyn_timer - Register a dynamic timer with the module
 * 
 * ARGS :-
 *   delay - dealy in ms
 *   func  - function pointer to invoke in delay ms
 *   data  - arg for the function pointer
 *
 * RET -
 *   Address of initialized timer object 
 */
timer_t *
add_dyn_timer(ub8 delay, timer_func func, ub8 data)
{
  timer_t *timer;

  if (delay == 0)
    return false;

  timer = (timer_t *)kmalloc_heap(sizeof(*timer));
  if (!timer)
    return NULL;

  timer->delay_timer = delay + ticks;
  timer->func_timer  = func;
  timer->data_timer  = data;

  add_dyn_timer_to_list(timer);
  
  /* 
   * We don't have a locking mechanism yet. So, we'll call
   * kheap_free from this context to avoid doing it in the
   * interrupt handler
   */
  free_dyn_deleted_timers();
}

/* 
 * EF: timer_exec - timer exec/callback
 * 
 * ARGS :-
 *   registers_t (see irq.h)
 *
 * RET
 */
void 
timer_exec(registers_t regs)
{
  ticks++;
  process_dyn_timers();
}

/* 
 * EF: timer_init_func - timer init function
 * 
 * ARGS :-
 *
 * RET - TRUE
 */
bool
timer_init_func()
{
  ub4 divisor;
  ub1 upper_byte;
  ub1 lower_byte;

  /* Initialize the global timer object */
  if (!init_dyn_timer())
    return false;

  /*
   * Let us first register the handler to be called on receiving IRQ0.
   * Because we have remapped the PIC, the IRQ0 from the PIT will be
   * delivered as INT 32 by MASTER PIC. On receiving INT 32, the processor
   * will look up the address at IDT[32]
   * IDT[32] -> irq0 -> irq_common -> irq_handler -> timer_exec
   */
  register_handler(IRQ0, timer_exec);
  
  /* see timer.h */
  divisor = 1193180 / FREQUENCY;

  /* 
   * Send the command byte. 0x36 sets the PIT to repeating mode (so that 
   * when the divisor counter reaches zero it's automatically refreshed)
   * and tells it we want to set the divisor value
   */
  port_byte_out(PIT_CMD, 0x36);

  // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
  lower_byte = (ub1)(divisor & 0xFF);
  upper_byte = (ub1)((divisor >> 8) & 0xFF);

  // Send the frequency divisor.
  port_byte_out(PIT_DATA, lower_byte);
  port_byte_out(PIT_DATA, upper_byte);
  printk_system("Initialized timer..");
  return true;
}

/* 
 * EF: timer_exit_func - timer exit function
 * 
 * ARGS :-
 *
 * RET -
 */
void
timer_exit_func()
{
  teardown_dyn_timer();
}

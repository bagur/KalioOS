/* KalioOS (C) 2020 Pranav Bagur */

#include "../common/if/common.h"
#include "../drivers/if/screen.h"
#include "if/isr.h"
#include "if/shell.h"
#include "../drivers/if/timer.h"
#include "../drivers/if/keyboard.h"
#include "../mm/if/paging.h"
#include "../mm/if/heap.h"
#include "../fs/if/fs.h"
#include "../test/if/tests.h"

#define TIMER_LOOP_MAGIC 0x4123

ub4 process = 1;

/* Driver init function pointers */
static bool (*_inits[])(void) = {
  screen_init_func,
  isr_init_func,
  paging_init_func,
  heap_init_func,
  timer_init_func,
  keyboard_init_func,
  fs_init_func,
  shell_init_func
};

/* Driver exit function pointers */
static void (*_exits[])(void) = {
  screen_exit_func,
  isr_exit_func,
  paging_exit_func,
  heap_exit_func,
  timer_exit_func,
  keyboard_exit_func,
  fs_exit_func,
  shell_exit_func
};


/*
 * EF: timer_callback - timer callback routine
 *
 * ARGS :-
 *
 * RET -
 */
void
timer_cb(ub8 data)
{
#ifdef DEBUG
  printk("callback called with data: ");
  printk_num((ub4)data);
  printk("\n");
#endif
  ASSERT (data == TIMER_LOOP_MAGIC);
  process = 1;
}

/* Kernel Entry */
void main()
{
  int i;

  /* Enable interrupts */
  asm volatile("sti");

  /* Initialize all our submodules */
  for (i = 0; i < ARRAY_SIZE(_inits) && _inits[i](); i++);

  /* Add a new line before control shell */
  printk_system(" ");

  while (true) {
    if (process) {
      process = 0;
      add_dyn_timer(30, &timer_cb, TIMER_LOOP_MAGIC);
      if (!shell_main())
        goto done;
    }
  }

done:
  while(i)
    _exits[i--]();
  PANIC("THAT'S ALL FOLKS!");
}


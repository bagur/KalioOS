/* KalioOS (C) 2020 Pranav Bagur */

/*
 * https://web.archive.org/web/20160327010849/
 * http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs and the PIT.html
 * The programmable interval timer is a chip connected to IRQ0. It can interrupt
 * the CPU at a user-defined rate (between 18.2Hz and 1.1931 MHz). The PIT is the
 * primary method used for implementing a system clock and the only method
 * available for implementing multitasking (switch processes on interrupt).
 * 
 * The PIT has an internal clock which oscillates at approximately 1.1931MHz. This
 * clock signal is fed through a frequency divider, to modulate the final output
 * frequency. It has 3 channels, each with it's own frequency divider.
 * 
 * Channel 0 is the most useful. It's output is connected to IRQ0.
 * 
 * OK, so we want to set the PIT up so it interrupts us at regular intervals, at
 * frequency f. To do this, we send the PIT a 'divisor'. This is the number that
 * it should divide it's input frequency (1.9131MHz) by
 */

#ifndef __TIMER_H
#define __TIMER_H

#include "../../common/if/types.h"
#include "../../common/if/list.h"
#include "port.h"
#include "../../kernel/if/isr.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
#define N_TIMER_LISTS 5

typedef void (*timer_func)(ub8);

/* STRUCT timer_t - Describes a single delay/timer request */
typedef struct _timer {
  list       link_timer;
  ub8        delay_timer;
  timer_func func_timer;
  ub8        data_timer;
} timer_t;

/* STRUCT timer_list_t - Describes global timer object */
typedef struct _list_timer {
  list lists_timer[N_TIMER_LISTS];
  ub4  lists_count_timer[N_TIMER_LISTS];

  list del_list_timer;
  ub4  del_list_count_timer;
} timer_list_t;

#define FREQUENCY 50 /* Hz */
#define PIT_CMD   0x43
#define PIT_DATA  0x40

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */
/* timer exec/callback */
void timer_exec(registers_t regs);

/* timer init function   */
bool timer_init_func(void);

/* timer exit function   */
void timer_exit_func(void);

/* Add a dynamic timer */
timer_t *add_dyn_timer(ub8 delay, timer_func func, ub8 data);

#endif

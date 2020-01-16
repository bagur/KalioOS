/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __TEST_H
#define __TEST_H

#include "../../common/if/types.h"
#include "../../common/if/list.h"
#include "../../mm/if/paging.h"
#include "../../mm/if/heap.h"
#include "../../drivers/if/timer.h"
#include "../../common/if/ring_buffer.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
typedef struct _list_test_st
{
  list link;
  ub1 val[20]; 
} list_test_st;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* cpu fault */
void test_cpu_fault(void);

/* page allocation and fault */
void test_page_fault(void);

/* list add/remove and loop */
void test_list(void);

/* heap malloc free grow shrink */
void test_heap(void);

/* timer callback */
void timer_callback(ub8 data);

/* test timer dyn */
void test_timer(void);

/* test lock code */
void test_lock(void);

/* test ring buffer */
void test_ring_buf(void);

#endif

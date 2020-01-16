/* KalioOS (C) 2020 Pranav Bagur */

/* This is just a wrapper around the list class */

#ifndef __STACK_H
#define __STACK_H

#include "types.h"
#include "list.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

typedef struct _stack_item 
{
  list        link_stack_item;
  void       *val_stack_item;
} stack_item_t;

typedef struct _stack 
{
  list        list_stack;
  ub4         count_stack;
} stack_t;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* initialize stack */
stack_t *stack_init();

/* push element to stack */
bool stack_push(stack_t *stack, void *val);

/* pop element from stack */
void *stack_pop(stack_t *stack);

/* teardown stack */
void stack_exit(stack_t *stack);

/* is stack empty? */
bool is_stack_empty(stack_t *stack);
#endif

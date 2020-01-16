/* KalioOS (C) 2020 Pranav Bagur */

#include "if/common.h"
#include "if/stack.h"

/* --------------------------------------------------------------------------
                         Export functions
   -------------------------------------------------------------------------- */
/*
 * EF: is_stack_empty - Check if stack empty
 *
 * ARGS :-
 *   stack - stack to teardown
 *
 * RET -
 *   TRUE iff stack is empty
 *   FALSE otherwise
 */
bool
is_stack_empty(stack_t *stack)
{
  return stack->count_stack == 0;
}

/*
 * EF: stack_push - Push element to stack
 *
 * ARGS :-
 *   stack - stack to teardown
 *   val   - element to push
 *
 * RET -
 */
bool
stack_push(stack_t *stack, void *val)
{
  stack_item_t *item;

  item = (stack_item_t *)kmalloc_heap(sizeof(*item));
  if (!item)
    return false;
  else {
    //memset((ub1 *)item, sizeof(*item), 0);
    item->val_stack_item = val;
    /* No need to lock - all the routines are called from the same
     * context for now
     */
    list_add_head(&stack->list_stack, &item->link_stack_item);
    stack->count_stack++;
    return true;
  }
}

/*
 * EF: stack_pop - Pop element from stack
 *
 * ARGS :-
 *   stack - stack to pop from
 *
 * RET -
 *   item pop'd
 */
void *
stack_pop(stack_t *stack)
{
  list         *item;
  void         *val = NULL;
  stack_item_t *stack_item;

  if (stack->count_stack > 0) {
    item  = list_remove_front(&stack->list_stack);
    stack->count_stack--;

    stack_item = list_entry(item, stack_item_t, link_stack_item);
    val = stack_item->val_stack_item;

    kfree_heap((ub4 *)stack_item);
  }

  return val;
}

/*
 * EF: stack_list - Initialize stack
 *
 * ARGS :-
 *
 * RET -
 */
stack_t *
stack_init()
{
  stack_t *stack;

  stack = (stack_t *)kmalloc_heap(sizeof(*stack));
  if (!stack)
    return NULL;
  else {
    //memset((ub1 *)stack, sizeof(*stack), 0);
    stack->count_stack = 0;
    list_init(&stack->list_stack);
  }

  return stack;
}

/*
 * EF: stack_exit - Teardown stack
 *
 * ARGS :-
 *   stack - stack to teardown
 *
 * RET -
 */
void
stack_exit(stack_t *stack)
{
  ASSERT(stack->count_stack == 0);
  kfree_heap((ub4 *)stack);
}

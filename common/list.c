/* KalioOS (C) 2020 Pranav Bagur */

#include "if/list.h"

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: init_list - Initialize list head
 * 
 * ARGS :-
 *   head - list head
 *
 * RET -
 */
void
list_init(list *head)
{
  head->next = NULL;
  head->prev = head;
}

/* 
 * EF: list_add_tail - add element to the end of the list
 * 
 * ARGS :-
 *   head - list head
 *   cur  - item to be added
 *
 * RET -
 */
void 
list_add_tail(list *head, list *cur)
{
  cur->next        = NULL;
  head->prev->next = cur;
  cur->prev        = head->prev;
  head->prev       = cur;
}

/* 
 * EF: list_add_head - add element to the start of the list
 * 
 * ARGS :-
 *   head - list head
 *   cur  - item to be added
 *
 * RET -
 */
void list_add_head(list *head, list *cur)
{
  cur->next        = head->next;
  if (cur->next)
    cur->next->prev  = cur;
  cur->prev        = head;
  head->next       = cur;
}

/* 
 * EF: list_remove - remove element from the list
 * 
 * ARGS :-
 *   cur  - item to be removed
 *
 * RET -
 */
void 
list_remove(list *head, list *cur)
{
  if (cur->next)
    cur->next->prev = cur->prev;
  else
    head->prev = cur->prev;
  cur->prev->next = cur->next;
}

/* 
 * EF: list_remove_front - remove first element from the list
 * 
 * ARGS :-
 *   head - list head
 *
 * RET -
 */
list * 
list_remove_front(list *head)
{
  list *cur = head->next;

  if (cur)
    list_remove(head, cur);

  return cur;
}

/* 
 * EF: is_list_empty - is list empty?
 * 
 * ARGS :-
 *   head - list head
 *
 * RET -
 *   true iff list is empty
 */
bool
is_list_empty(list *head)
{
  return (head->prev == head);
}

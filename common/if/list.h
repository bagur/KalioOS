/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __LIST_H
#define __LIST_H

#include "types.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
typedef struct _list
{
  struct _list *next;
  struct _list *prev;
} list;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
#define list_entry(link, type, member)                                        \
  ((type *)((char *)(link)-(unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head)                                              \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = (pos) ? pos->next: NULL; pos != NULL;        \
         pos = n, n = pos->next)

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* initialize list */
void list_init(list *head);

/* add element to list (tail) */
void list_add_tail(list *head, list *cur);

/* add element to list (head) */
void list_add_head(list *head, list *cur);

/* remove element from list */
void list_remove(list *head, list *cur);

/* remove first element from list */
list *list_remove_front(list *head);

/* is list empty? */
bool is_list_empty(list *head);
#endif

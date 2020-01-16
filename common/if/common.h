/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __COMMON_H
#define __COMMON_H

#include "types.h"
#include "../../drivers/if/screen.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
#define USERNAME  "pbagur"
#define GROUPNAME "adm"

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

#define EXIT()                                                                \
  do {                                                                        \
    erase_cursor();                                                           \
    printk_attr("System shutting down\n", RED_ON_BLACK);                      \
    erase_cursor();                                                           \
    printk_attr("Goodbye!\n", RED_ON_BLACK);                                  \
    erase_cursor();                                                           \
    while(1);                                                                 \
  } while (0)

#define ASSERT(x)                                                             \
  do {                                                                        \
    if (!x)                                                                   \
      printk(#x);                                                             \
  } while (0)

#define PANIC(x)                                                              \
  do {                                                                        \
    printk_attr("!!!SYSTEM PANIC!!!\n", RED_ON_BLACK);                        \
    printk_attr(#x, RED_ON_BLACK);                                            \
    while(1);                                                                 \
  } while (0)

#define ARRAY_SIZE(_arr) sizeof(_arr)/sizeof(_arr[0])

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* return the number of digits in a number */
ub4 get_num_digits(ub4 num);

/* convert number to string */
void convert_to_str(ub4 num, char *str, ub4 len);
#endif

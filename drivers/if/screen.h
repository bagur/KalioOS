/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __SCREEN_H
#define __SCREEN_H

#include "../../common/if/types.h"
#include "port.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

#define VIDEO_MEMORY    0xb8000
#define MAX_ROWS        25
#define MAX_COLS        80
#define WHITE_ON_BLACK  0x0f
#define RED_ON_WHITE    0xf4
#define RED_ON_BLACK    0x4
#define GREEN_ON_BLACK  0x0a
#define GRAY_ON_BLACK   0x08

/* Screen I/O ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#define PROMPT_CHAR     '$'

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

#define printk_char(_char)                                                     \
  printk_char_attr(_char, WHITE_ON_BLACK); 

#define printk(_msg)                                                           \
  printk_attr(_msg, WHITE_ON_BLACK); 

#define printk_num(_num)                                                       \
  printk_num_attr(_num, WHITE_ON_BLACK); 

#define printk_shell(_msg)                                                     \
  printk_attr(_msg, GREEN_ON_BLACK); 

#define printk_shell_num(_num)                                                 \
  printk_num_attr(_num, GREEN_ON_BLACK); 


/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 

/* clear VGA screen  */
void clear_screen();

/* print char at cursor position  */
void printk_char_attr(const ub1 char_to_print, ub4 attr);

/* print string at cursor position   */
void printk_attr(const ub1 *msg, ub4 attr);

/* erase cursor */
void erase_cursor(void);

/* print a system message */
void printk_system(const ub1 *msg);

/* respond to backspace keydown */
void event_backspace_down(void);

/* respond to up arrow keydown */
void event_uparrow_down(void);

/* respond to down arrow keydown */
void event_downarrow_down(void);

/* respond to left arrow keydown */
void event_leftarrow_down(void);

/* respond to right arrow keydown */
void event_rightarrow_down(void);

/* clear screen and print a prompt */
void clear_screen_prompt(void);

/* driver init function   */
bool screen_init_func(void);

/* driver exit function   */
void screen_exit_func(void);

#endif

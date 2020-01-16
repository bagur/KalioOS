/* KalioOS (C) 2020 Pranav Bagur */

#include "if/screen.h"
#include "../common/if/common.h"

/* -------------------------------------------------------------------------- 
                         Static function declarations
   -------------------------------------------------------------------------- */ 

static void set_cursor_position(ub4 new_pos);
static ub4  get_cursor_position(void);

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

/* Number of chars on the current line */
ub4 cur_shell_row = 0;
ub4 cur_shell_col = 0;

/* -------------------------------------------------------------------------- 
                         Static inline functions
   -------------------------------------------------------------------------- */ 

/* === SIF: cursor offset to VGA buffer offset === */
static inline ub4
cur_to_vga(ub4 offset)
{
  /*
   * The starting address of the memory-mapped I/O for the framebuffer is
   * 0x000B8000 [27]. The memory is divided into 16 bit cells, where the 16
   * bits determine both the character, the foreground color and the background
   * color. The highest eight bits is the ASCII [28] value of the character,
   * bit 7 - 4 the background and bit 3 - 0 the foreground
   */
  return (offset * 2);
}

/* === SIF: VGA buffer offset to cursor offset === */
static inline ub4
vga_to_cur(ub4 offset)
{
  return (offset / 2);
}

/* === SIF: VGA buffer offset from row and col number === */
static inline ub4
get_vga_offset(ub4 row, ub4 col)
{
  return (2 * (row * MAX_COLS + col)); 
}

/* === SIF: return row number from VGA buffer offset === */
static inline ub4
get_row_from_offset(ub4 offset)
{
  ub4 idx = (offset / 2);

  return (ub4) (idx / MAX_COLS);
}

/* === SIF: return col number from VGA buffer offset === */
static inline ub4
get_col_from_offset(ub4 offset)
{
  ub4 idx = (offset / 2);

  return (ub4) (idx % MAX_COLS);
}

/* === SIF: ret the current charecter at offset === */
static inline ub1
get_char_at_offset(ub4 offset)
{
  ub1 *vga = (ub1 *)VIDEO_MEMORY;

  return vga[offset];
}

/* === SIF: Are we on a new line? === */
static inline bool
is_new_line()
{
  ub4  cur_pos    = get_cursor_position();
  ub4  vga_offset = cur_to_vga(cur_pos);

  vga_offset -= 2;
  return (get_char_at_offset(vga_offset) == PROMPT_CHAR);
}

/* === SIF: print char at VGA buffer offset === */
static inline void
print_char(ub1 char_to_print, ub4 offset, ub4 attr)
{
  ub1 *vga = (ub1 *)VIDEO_MEMORY;

  vga[offset] = char_to_print;
  vga[offset + 1] = attr;
}

/* === SIF: print prompt at cursor position === */
static inline void
print_prompt()
{
  printk_char_attr(PROMPT_CHAR, WHITE_ON_BLACK);
}

/* === SIF: move cursor to the next row === */
static inline void 
move_cur_to_next_row() 
{
  ub4 row = get_row_from_offset(cur_to_vga(get_cursor_position()));  
  
  set_cursor_position(vga_to_cur(get_vga_offset(row + 1,0)));
  print_prompt();
}

/* === SIF: check if col position has overflowed === */
static inline bool 
col_overflow(ub4 vga_offset) 
{
  ub4 col = get_col_from_offset(vga_offset);

  if (col >= MAX_COLS)
    return true;

  return false;
}

/* -------------------------------------------------------------------------- 
                         Static functions
   -------------------------------------------------------------------------- */ 

/* 
 * SF: get_cursor_position - Get the current cursor position
 * 
 * ARGS :-
 *
 * RET - position of the cursor
 */
static ub4
get_cursor_position()
{
  ub4 cursor_pos;

  /* 
   * Screen cursor position: ask VGA control register (0x3d4) for bytes
   * 14 = high byte of cursor and 15 = low byte of cursor. 
   */

  /* Requesting byte 14: high byte of cursor pos */
  port_byte_out(REG_SCREEN_CTRL, 14);   
  /* Data is returned in VGA data register (0x3d5) */
  cursor_pos = port_byte_in(REG_SCREEN_DATA);
  cursor_pos = (cursor_pos << 8); /* high byte */

  port_byte_out(REG_SCREEN_CTRL, 15); /* requesting low byte */
  cursor_pos += port_byte_in(REG_SCREEN_DATA);

  return cursor_pos;
}

/* 
 * SF: set_cursor_position - Set the current cursor position
 * 
 * ARGS :-
 *   new_pos - position to set the cursor at
 *
 * RET
 */
static void 
set_cursor_position(ub4 new_pos)
{
  ub1 high_pos = (ub1)(new_pos >> 8);
  ub1 low_pos  = (ub1)(new_pos & 0xff);

  /* 
   * Screen cursor position: ask VGA control register (0x3d4) for bytes
   * 14 = high byte of cursor and 15 = low byte of cursor. 
   */
  /* Set high byte */
  port_byte_out(REG_SCREEN_CTRL, 14);   
  port_byte_out(REG_SCREEN_DATA, high_pos);

  /* Set low byte */
  port_byte_out(REG_SCREEN_CTRL, 15);
  port_byte_out(REG_SCREEN_DATA, low_pos);
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 

/* 
 * TODO: 
 * Gray on black by default. arrow keys can only move between white background
 * spaces.
 * If a new charecter is added in an already white space, move everything to the
 * right
 */

/* 
 * EF: clear_screen - clear VGA screen
 * 
 * ARGS :-
 *
 * RET -
 */
void clear_screen()
{
  ub4  i   = 0;
  ub4  j   = 0;

  for (i = 0; i < MAX_ROWS; i++) {
    for (j = 0; j < MAX_COLS; j++) {
      print_char(' ', get_vga_offset(i, j), WHITE_ON_BLACK);
    }
  }

  set_cursor_position(vga_to_cur(get_vga_offset(0,0)));
}

/* 
 * EF: printk_char_attr - print char at cursor position 
 * 
 * ARGS :-
 *   char_to_print - char to print
 *   attr - color attributes
 *
 * RET -
 */
void
printk_char_attr(const ub1 char_to_print, ub4 attr)
{
  ub4 cur_pos    = get_cursor_position();
  ub4 vga_offset = cur_to_vga(cur_pos);

  /* 
   * Dont have to worry about column overflow - the
   * VGA address layout is linear
   */
  print_char(char_to_print, vga_offset, attr);
  vga_offset += 2;
  set_cursor_position(vga_to_cur(vga_offset));
}

/* 
 * EF: printk_attr - print string at cursor position 
 * 
 * ARGS :-
 *   msg - string to print
 *   attr - color attributes
 *
 * RET -
 */
void 
printk_attr(const ub1 *msg, ub4 attr)
{
  while (*msg != 0) {
    
    if (*msg == '\n') 
      move_cur_to_next_row();
    else
      printk_char_attr(*msg, attr);

    msg++;
  }
}

/* 
 * EF: printk_num_attr - print num at cursor position 
 * 
 * ARGS :-
 *   num -  number to print
 *   attr - color attributes
 *
 * RET -
 */
void 
printk_num_attr(ub4 num, ub4 attr)
{
  ub4 digits = get_num_digits(num) ?: 1;
  ub1 str[digits + 1];

  convert_to_str(num, str, digits);
  str[digits] = '\0';
  printk_attr(str, attr);
}

/* 
 * EF: erase_cursor - Erase the cursor
 * 
 * ARGS :-
 *
 * RET -
 */
void 
erase_cursor()
{
  ub4  cur_pos;
  ub4  vga_offset;

  /* 
   * If we are not on a new line, move to the next
   * line
   */
  if (!is_new_line()) {
    move_cur_to_next_row();
  }
 
  /* Erase the prompt for system messages */
  cur_pos     = get_cursor_position();
  vga_offset  = cur_to_vga(cur_pos);
  vga_offset -= 2;
  print_char(' ', vga_offset, WHITE_ON_BLACK);
  set_cursor_position(vga_to_cur(vga_offset));
}

/* 
 * EF: printk_system - print a system message
 * 
 * ARGS :-
 *   msg - string to print
 *
 * RET -
 */
void 
printk_system(const ub1 *msg)
{
  ub4  cur_pos;
  ub4  vga_offset;

  /* 
   * If we are not on a new line, move to the next
   * line
   */
  if (!is_new_line()) {
    move_cur_to_next_row();
    printk_system(msg);
    goto done;
  }
 
  /* Erase the prompt for system messages */
  cur_pos     = get_cursor_position();
  vga_offset  = cur_to_vga(cur_pos);
  vga_offset -= 2;
  print_char(' ', vga_offset, WHITE_ON_BLACK);
  set_cursor_position(vga_to_cur(vga_offset));

  /* Print the message */
  printk_attr(msg, GREEN_ON_BLACK);

  /* Check if we on a new line. Else move to the next line */
  if (!is_new_line()) {
    move_cur_to_next_row();
  }

done:
  return;
}

/* 
 * EF: event_backspace_down - respond to backspace keydown 
 * 
 * ARGS :-
 * RET -
 */
void event_backspace_down() 
{
  ub4  cur_pos;
  ub4  vga_offset;

  /* If we are on a new line, ignore the backspace request */
  if (is_new_line())
    goto done;

  cur_pos     = get_cursor_position();
  vga_offset  = cur_to_vga(cur_pos);
  vga_offset -= 2;
  print_char(' ', vga_offset, WHITE_ON_BLACK);
  set_cursor_position(vga_to_cur(vga_offset));

done:
  return;
}

/* 
 * EF: event_uparrow_down - respond to up arrow keydown 
 * NOTE: it is the shell's responsibility to not move the cursor
 * beyond the current line. Our task is to make sure we stay
 * within the screen limits
 * 
 * ARGS :-
 * RET -
 */
void event_uparrow_down() {
  ub4 vga_offset = cur_to_vga(get_cursor_position());
  ub4 row = get_row_from_offset(vga_offset);  
  ub4 col = get_col_from_offset(vga_offset);  

  if (row > 0) 
    set_cursor_position(vga_to_cur(get_vga_offset(row - 1, col)));
}

/* 
 * EF: event_downarrow_down - respond to down arrow keydown 
 * 
 * ARGS :-
 * RET -
 */
void event_downarrow_down()
{
  ub4 vga_offset = cur_to_vga(get_cursor_position());
  ub4 row = get_row_from_offset(vga_offset);  
  ub4 col = get_col_from_offset(vga_offset);  

  if (row < (MAX_ROWS - 1)) 
    set_cursor_position(vga_to_cur(get_vga_offset(row + 1, col)));
}

/* 
 * EF: event_leftarrow_down - respond to left arrow keydown 
 * 
 * ARGS :-
 * RET -
 */
void event_leftarrow_down()
{
  ub4 vga_offset = cur_to_vga(get_cursor_position());
  ub4 row = get_row_from_offset(vga_offset);  
  ub4 col = get_col_from_offset(vga_offset);  

  if (col > 0) 
    set_cursor_position(vga_to_cur(get_vga_offset(row, col - 1)));
}

/* 
 * EF: event_rightarrow_down - respond to right arrow keydown 
 * 
 * ARGS :-
 * RET -
 */
void event_rightarrow_down()
{
  ub4 vga_offset = cur_to_vga(get_cursor_position());
  ub4 row = get_row_from_offset(vga_offset);  
  ub4 col = get_col_from_offset(vga_offset);  

  if (col < (MAX_COLS - 1)) 
    set_cursor_position(vga_to_cur(get_vga_offset(row, col + 1)));
}

/* 
 * EF: clear_screen_prompt - clear screen and print a prompt
 * 
 * ARGS :-
 * RET -
 */
void clear_screen_prompt()
{
  clear_screen();
  print_prompt();
}

/* 
 * EF: screen_init_func - driver init function
 * 
 * ARGS :-
 *
 * RET - TRUE
 */
bool
screen_init_func()
{
  clear_screen();
  print_prompt();

  /* 
   * Screen init funtion is unique because we clear the screen
   * and thus lose all our messages from before.
   * Therefore, we'll start by re-logging a few boot messages
   */
  printk_system("Welcome to KalioOS!");
  printk_system("Started in 16-bit real mode..");
  printk_system("Loaded kernel into memory..");
  printk_system("Switched to 32-bit protected mode..");
  printk_system("Initialized screen driver..");

  return true;
}

/* 
 * EF: screen_exit_func - driver exit function
 * 
 * ARGS :-
 *
 * RET -
 */
void
screen_exit_func()
{
}

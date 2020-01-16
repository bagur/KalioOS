/* KalioOS (C) 2020 Pranav Bagur */

#include "if/keyboard.h"
#include "../kernel/if/isr.h"
#include "if/screen.h"
#include "../common/if/ring_buffer.h"

ring_buf *rb_keyboard;
const ub1 *keyboard_map[128] =
{
   0,  0, "1", "2", "3", "4", "5", "6", "7", "8",
  "9", "0", "-", "=", "\b", "\t", "q", "w", "e", "r",
  "t", "y", "u", "i", "o", "p", "[", "]", "\n", 0,
  "a", "s", "d", "f", "g", "h", "j", "k", "l", ";",
  "\"", "`", 0, "\\", "z", "x", "c", "v", "b", "n",
  "m", ",", ".", "/", 
    0,	/* Right shift */
  "*",
    0,	/* Alt */
  " ",	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  "-",
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  "+",
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0, 0, 0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* 
 * EF: keyboard_exec - keyboard exec/callback
 * 
 * ARGS :-
 *   registers_t (see irq.h)
 *
 * RET
 */
void 
keyboard_exec(registers_t regs)
{
  ub8 scancode = port_byte_in(KEYBOARD_DATA);

  if (!(scancode & 0x80) && scancode >= 0 && scancode < 128) {
    /* 
     * Key Down
     * if you hold a key down, you will get repeated key press
     * interrupts. 
     */
    switch (scancode) {
      case BACKSPACE_SCCODE:
      {
        const ub1 *bsstr = keyboard_map[scancode];

        if (bsstr) {
          rb_push(rb_keyboard, (void *)bsstr);
        }
        event_backspace_down();
        break;
      }
      case UP_SCCODE:
        //event_uparrow_down();
        break;
      case DOWN_SCCODE:
        //event_downarrow_down();
        break;
      case LEFT_SCCODE:
        //event_leftarrow_down();
        break;
      case RIGHT_SCCODE:
        //event_rightarrow_down();
        break;
      default:
      {
        const ub1 *str = keyboard_map[scancode];

        if (str) {
          printk(str);
          rb_push(rb_keyboard, (void *)str);
        }
        break;
      }
    }
  }
}

/* 
 * EF: keyboard_init_func - keyboard init function
 * 
 * ARGS :-
 *
 * RET - TRUE
 */
bool
keyboard_init_func()
{
  register_handler(IRQ1, keyboard_exec);
  /* Order doesn't matter, we haven't enabled interrupts yet */
  rb_keyboard = rb_init(sizeof(ub1), KEYBOARD_RING_BUF_MAX);
  if (!rb_keyboard)
    return false;

  printk_system("Initialized keyboard..");
  return true;
}

/* 
 * EF: keyboard_exit_func - keyboard exit function
 * 
 * ARGS :-
 *
 * RET -
 */
void
keyboard_exit_func()
{
  rb_free(rb_keyboard);
}

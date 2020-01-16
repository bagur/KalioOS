/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "../../common/if/types.h"
#include "port.h"
#include "../../kernel/if/isr.h"
#include "../../common/if/ring_buffer.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

#define KEYBOARD_DATA         0x60

#define BACKSPACE_SCCODE      0x0E
#define UP_SCCODE             0x48
#define DOWN_SCCODE           0x50
#define LEFT_SCCODE           0x4B
#define RIGHT_SCCODE          0x4D

#define KEYBOARD_RING_BUF_MAX 0x800

extern const ub1 *keyboard_map[128];
extern ring_buf *rb_keyboard;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */
/* keyboard exec/callback */
void keyboard_exec(registers_t regs);

/* keyboard init function   */
bool keyboard_init_func(void);

/* keyboard exit function   */
void keyboard_exit_func(void);

#endif

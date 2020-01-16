/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __LOCK_INTR_H
#define __LOCK_INTR_H

#include "types.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* Synchronize with the interrupt context */

/* Disable interrupts  */
bool lock_intr(ub8 *flags);

/* Enable interrupts  */
void unlock_intr(ub8 *flags);
#endif

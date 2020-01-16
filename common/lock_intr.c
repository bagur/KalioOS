/* KalioOS (C) 2020 Pranav Bagur */

#include "if/lock_intr.h"
#include "if/common.h"

/* === SIF: Disable interrupts === */
inline ub8 disable_intr()
{
  ub8 flags;
  asm volatile("pushfl; pop %0; cli;" : "=g" (flags));

  return flags;
}

/* === SIF: Enable interrupts === */
inline ub8 enable_intr()
{
  ub8 flags;

  asm volatile("push %0; popfl; sti;" :: "g" (flags));
  return flags;
}

/* === SIF: Check if interrupts are enabled === */
inline bool 
interrupts_enabled()
{
  ub8 flags;

  asm volatile("pushfl; pop %0;" : "=g" (flags));
  return !!(flags & 0x200);
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: lock_intr - disable interrupts
 * 
 * ARGS :-
 *   flags - see disable_intr above
 *
 * RET -
 *   TRUE  - iff interrupts are not already disabled
 *   FALSE - otherwise
 */
bool lock_intr(ub8 *flags)
{
  if (!interrupts_enabled())
    return false;

  *flags = disable_intr();
  return true;
}

/* 
 * EF: unlock_intr - enable interrupts
 * 
 * ARGS :-
 *   flags - see enable_intr above
 *
 * RET -
 */
void 
unlock_intr(ub8 *flags)
{
  if (interrupts_enabled()) {
    PANIC("INTR ENABLED");
  }

  *flags = enable_intr();
}

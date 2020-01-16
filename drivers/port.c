/* KalioOS (C) 2020 Pranav Bagur */

#include "if/port.h"

/* 
 * EF: port_byte_in - Read a byte from the specified port number
 * 
 * ARGS :-
 * port_num - port number to read from 
 *
 * RET - byte read
 */
ub1 
port_byte_in(ub2 port_num)
{
  ub1 rval;

  /*
   * % is used to denote registers
   * Inline assembler syntax
   * !! Notice how the source and destination registers are switched from NASM !!
   *
   * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
   * '"d" (port)': map the C variable '(port)' into e'd'x register
   *
   * Inputs and outputs are separated by colons
   */
   __asm__("in %%dx, %%al" : "=a" (rval) : "d" (port_num));
   return rval;
}

/* 
 * EF: port_byte_out - Write a byte to the specified port number
 * 
 * ARGS :-
 * port_num - port number to write to
 * data     - data to write
 */
void 
port_byte_out(ub2 port_num, ub1 data)
{
  /* Notice how here both registers are mapped to C variables and
  * nothing is returned, thus, no equals '=' in the asm syntax 
  * However we see a comma since there are two variables in the input area
  * and none in the 'return' area
  */
  __asm__("out %%al, %%dx" : : "a" (data), "d" (port_num));
}

/* 
 * EF: port_word_in - Read a word from the specified port number
 * 
 * ARGS :-
 * port_num - port number to read from 
 *
 * RET - word read
 */
ub2 
port_word_in(ub2 port_num)
{
   ub2 rval;

   __asm__("in %%dx, %%ax" : "=a" (rval) : "d" (port_num));
   return rval;
}

/* 
 * EF: port_word_out - Write a word to the specified port number
 * 
 * ARGS :-
 * port_num - port number to write to
 * data     - data to write
 */
void 
port_word_out(ub2 port_num, ub2 data)
{
  __asm__("out %%ax, %%dx" : : "a" (data), "d" (port_num));
}


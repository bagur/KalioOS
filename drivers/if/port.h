/* KalioOS (C) 2020 Pranav Bagur */

/* 
 * Go to the follwing links to understand port I/O
 * 
 * https://www.youtube.com/watch?v=yTJXko0F46U
 * https://littleosbook.github.io/#interacting-with-the-hardware
 *
 * There are usually two different ways to interact with the hardware,
 * memory-mapped I/O and I/O ports.
 *
 * If the hardware uses memory-mapped I/O then you can write to a specific
 * memory address and the hardware will be updated with the new data. One
 * example of this is the framebuffer, which will be discussed in more detail
 * later. For example, if you write the value 0x410F to address 0x000B8000, you
 * will see the letter A in white color on a black background (see the section
 * on the framebuffer for more details).
 *
 *                    0xfffff  +----------------------+
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |     Reserved for     |
 *                             |     memory           |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             +----------------------+
 *                +----------+ |                      |
 *    Device 'k'  +----------+ |                      |
 *    Harddisk    +----------+ |    Reserved for      |
 *                             |    I/O               |
 *    Framebuffer              |                      |
 *                +----------+ +----------------------+
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |    Reserved for      |
 *                             |    memory            |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                             |                      |
 *                    0x00000  +----------------------+
 *
 *                             Physical address space
 *
 *
 * If the hardware uses I/O ports then the assembly code instructions out and in
 * must be used to communicate with the hardware. The instruction out takes two
 * parameters: the address of the I/O port and the data to send. The instruction
 * in takes a single parameter, the address of the I/O port, and returns data
 * from the hardware. One can think of I/O ports as communicating with hardware
 * the same way as you communicate with a server using sockets. The cursor (the
 * blinking rectangle) of the framebuffer is one example of hardware controlled
 * via I/O ports on a PC.
 */
#ifndef __PORT_H
#define __PORT_H

#include "../../common/if/types.h"

ub1 port_byte_in(ub2 port_num);
void port_byte_out(ub2 port_num, ub1 data);
ub2 port_word_in(ub2 port_num);
void port_word_out(ub2 port_num, ub2 data);

#endif

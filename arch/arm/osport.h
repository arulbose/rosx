/* RosX RT-Kernel
 * Copyright (C) 2016 Arul Bose<bose.arul@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Reference: arch/x86/include/asm/switch_to.h */
#ifndef __OSPORT_H__
#define __OSPORT_H__

#include "irq_mapping.h"

#define _ASMLANGUAGE

void __switch_to(void);

#define __sched__(prev, next) 							\
do {										\
} while (0)									

#define __init_tcb__(tcb) 							\
do {										\
} while (0)

#define __preempt__(tcb)                                                        \
do {                                                                            \
}while(0)

/* OS port hooks */
unsigned int rx_enter_critical (void);
void rx_exit_critical (unsigned int);

#define  __printk(...)  {}

volatile int __context; /* user:0/IRQ:1 to be used by the port critical section code to bypass if already in IRQ context */
volatile int __critical;

#define __IRQ 1
#define __USER 0

#endif

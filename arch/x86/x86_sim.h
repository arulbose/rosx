/* Rose RT-Kernel
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

#ifndef __X86_SIM_H__ 
#define __X86_SIM_H__

#include <stdio.h>
#include <stdlib.h>
#include <rose/string.h>
#include <signal.h>
#include "irq_mapping.h"

#define  __printk(...) 	\
			{ \
			printf(__VA_ARGS__); \
			fflush(stdout); \
			}


#define  __early_printk(...) 	\
			{ \
			printf(__VA_ARGS__); \
			fflush(stdout); \
			}

sigset_t mask;
sigset_t orig_mask;
struct sigaction act;
volatile int __context; /* user:0/IRQ:1 to be used by the port critical section code to bypass if already in IRQ context */
volatile int __critical;

#define __IRQ 1
#define __USER 0

#endif 

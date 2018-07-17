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

/* Reference: Linux src : arch/x86/include/asm/switch_to.h */

#ifndef __x86_SWITCH_H__
#define __x86_SWITCH_H__

#include "x86_sim.h"

#define _ASMLANGUAGE

void __switch_to(void);

#define __sched__(prev, next) 							\
do {										\
 	/*                                                              	\
         * Context-switching clobbers all registers, so we clobber      	\
         * them explicitly, via unused output variables.                	\
         * (EAX and EBP is not listed because EBP is saved/restored     	\
         * explicitly for wchan access and EAX is the return value of   	\
         * __switch_to())                                               	\
         */                                                             	\
										\
 __asm__ volatile("pushfl\n\t"                	/* save    flags */  		\
	      "pushl %%ebp\n\t"           	/* save    EBP   */    		\
	      "movl %%esp,%[prev_sp]\n\t" 	/* save    ESP   */    		\
	      "movl $1f,%[prev_ip]\n\t"   	/* save    EIP   */    		\
	      "movl %[next_sp],%%esp\n\t"       /* restore ESP   */ 		\
	      "popl %%ebp\n\t"           	/* restore EBP   */     	\
              "popfl\n\t"                  	/* restore flags */     	\
              "pushl %[next_ip]\n\t"     	/* restore EIP   */     	\
	      "jmp __rx_switch_to\n\t"	        /* call the C function */	\
	      "1:\t"								\
										\
  		/* output parameters */         		                \
	      : [prev_sp] "=m" (prev->curr_stack_ptr),                		\
	        [prev_ip] "=m" (prev->ip)					\
					                			\
	       /* input parameters: */                          		\
     	      : [next_sp]  "m" (next->curr_stack_ptr),                		\
       		[next_ip]  "m" (next->ip),	           			\
		[next_bp]  "m" (next->bp));					\
} while (0)									

#endif

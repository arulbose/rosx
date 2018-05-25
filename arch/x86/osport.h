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

#ifndef __x86_PORT_H__
#define __x86_PORT_H__
#include "x86_sim.h"

#define _ASMLANGUAGE

#define __init_tcb__(tcb) 							\
do {										\
   __asm__ volatile("pushl %%ebp\n\t"	    /* save caller  EBP */		\
		"movl %%esp,%[curr_sp_o]\n\t" /* save esp to current_sp_o */	\
		"movl %[tcb_sp],%%esp\n\t"  /* store task SP to ESP */          \
                "movl %[tcb_sp],%%ebp\n\t"  /* store task SP to EBP */          \
                "pushfl\n\t"                                               	\
                "pushl %%ebp\n\t"           /* save EBP in task stack   */      \
                "movl %%esp,%[tcb_sp_o]\n\t"  /* save ESP to task cur pointer*/ \
                "movl %[curr_sp],%%esp\n\t" /* restore ESP of the caller */     \
                "popl %%ebp\n" 		                                 	\
										\
		/* Output */							\
		:[tcb_sp_o]  "=m" (tcb->curr_stack_ptr),                        \
                 [curr_sp_o] "=m" (tcb->ip)					\
                /* input */	                                                \
                :[tcb_sp]  "m" (tcb->stack_start_ptr),                          \
                 [curr_sp] "m" (tcb->ip));                          		\
} while (0)

#define __preempt__(tcb)                                                        \
do {                                                                            \
        __asm__ volatile("movl 68(%%ebp), %%ebx\n\t" /* copy the ebp+68 to ebx to get the EIP location of the thread when interrupted from the stack(kernel signal code stores the interrupted eip ptr at ebp+68(fix it to be more reliable ??)) */        \
                         "movl %%ebx, %[tcb_p]\n\t"  /* mov @ebx(actual EIP pointer) to tcb->preempt  */                                                \
                         "movl $__preempt_task, 68(%%ebp)\n\t"  /* mov preempt_task to the prev EIP pointer to redirect to preempt function call  */                                            \
                         :[tcb_p] "=m" (tcb->preempt)                            \
                         :                                                       \
                         :"ebx"                 /* Clobbered */                  \
                  );                                                             \
}while(0)

/* IRQ control */
void __restore_irq_global(unsigned int key);
unsigned int __disable_irq_global(void);
void __enable_irq(unsigned int);
void __disable_irq(unsigned int);

#endif

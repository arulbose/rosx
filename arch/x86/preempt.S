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


#define _ASMLANGUAGE

	.text
	.global __preempt_task

__preempt_task:
		#save the EIP where the task for preempted <start>
		movl %eax, -8(%esp)   # save the eax on the stack
                movl __curr_running_task, %eax # copy the current runing task to eax
                movl 24(%eax), %eax # copy the __curr_running_task->preempt to eax which has the EIP of the preempted location
                push %eax # push the EIP to stack
                movl -4(%esp), %eax # restore eax
		#save the EIP where the task for preempted <end>

		# Save all the registers to preserve context
		pushl %eax
		pushl %ebx
		pushl %ecx
		pushl %edx
		pushl %esi
		pushl %edi
		pushfl

		call rose_sched # C funtion in sched.c
		
		# Restore context
		popfl
		popl %edi	
		popl %esi
		popl %edx	
		popl %ecx	
		popl %ebx	
		popl %eax	

		ret # return to the EIP stored in the stack
			

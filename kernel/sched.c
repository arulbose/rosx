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

#include <RoseRTOS.h>

void rose_sched()
{
	unsigned int a = 0;

	if((__curr_running_task != __task_ready_head)){
	  __need_resched = 0;
	  a = enter_critical();
	  __sched__(__curr_running_task, __task_ready_head); /* port specific call */ 
	}
}

/* Up the current running task */
void __switch_to()
{
        __curr_running_task = __task_ready_head;
	__curr_running_task->state = TASK_RUNNING;
#ifdef CONFIG_TIME_SLICE
	if(__curr_running_task->time_slice && !__curr_running_task->ticks) { /* time_slice should be greater than 0 and ticks should be 0 to reload */
		__curr_running_task->ticks = __curr_running_task->time_slice; /* Reset the time slice */
	}
#endif
	exit_critical(1);
        return;
}


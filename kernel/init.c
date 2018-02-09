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

static TCB timer0_tcb;
static TCB event0_tcb;
static TCB logger0_tcb;

/* App entry point */
extern void application_init(void);

/* First OS call made after platform initialization 
*  All OS specific initialization done here 
*/
void __kernel_enter()
{
    /* mempool init */
    bytepool_init();
    /* stack init */
    __curr_stack_ptr = __stack_start_ptr; 
    /* init task(just a place holder needed for the first context switch ) */
    strcpy(__init_task.name, "init"); 
    __curr_running_task = &__init_task;
    __curr_num_task = 0;

    /* create all system threads */
    create_task(&timer0_tcb,"timer0", 0, 0, 8192, rose_timer_thread, TASK_READY, 0); /* system timer thread */
    create_task(&event0_tcb,"event0", 0, 0, 8192, rose_event_thread, TASK_READY, 0); /* system event thread */
            
    /* Logger init */
    __printk_buffer_head = __printk_buffer_tail = __printk_buffer_start_ptr;
    create_task(&logger0_tcb,"logger0", (LEAST_PRIO - 1), 0, 8192, rose_logger_thread, TASK_READY, 0); /* system logger thread */
    
    application_init();	
    /* NO RETURN */
}


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

/* Rose Wait Q
 * */

int add_to_wait_queue(struct wait_queue *wq, int task_state)
{
    TCB *tmp;

    unsigned int imask = enter_critical();
    
    __curr_running_task->state = task_state;
    __curr_running_task->wq = wq;
    remove_from_ready_q(__curr_running_task);
    __curr_running_task->next = NULL;
  
    wq->task = __curr_running_task;

    /* Add the waiting task to the sys wait queue */    
    if(!(__sys_wait_list)) {
        __sys_wait_list = wq;
    }else{
        /* add wait queue to the tail of sytem wait list */
        tmp = __sys_wait_list;
        while(tmp->next){
            tmp = tmp->next;
        }
        tmp->next = wq;
        wq->prev = tmp;
    }

    exit_critical(imask);

    return 0;
}

int wakeup(struct wait_queue *wq)
{
    unsigned int imask = enter_critical();
    
    if(!__sys_wait_list)
        return -ENXIO;

    if(__sys_wait_list == wq) {
        /* The first node is the task to be removed */
        __sys_wait_list = wq->next;
    }else{
        /* Cut and connect the chain */
        wq->prev->next = wq->next;
        wq->next->prev = wq->prev;
    }
 
    exit_critical(imask);
    return 0;
}

void rose_event_thread()
{
   
    while(1) {
    /* wake up only threads which are TASK_INTERRUPTIBLE */

    }
}

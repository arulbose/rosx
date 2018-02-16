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

int add_to_wait_queue(struct wait_queue *wq)
{
    TCB *tmp;

    unsigned int imask = enter_critical();
    
    __curr_running_task->state = TASK_SUSPEND;
    __curr_running_task->wq = wq;
    remove_from_ready_q(__curr_running_task);
    __curr_running_task->next = NULL;
  
    if(!(wq->task)) {
        wq->task = __curr_running_task;
    
    if(!(__sys_wait_list->next)) {
        __sys_wait_list->next = __curr;
    }else{
        tmp = __sys_wait_list->task;
        while(tmp){
            tmp = tmp->next;
        }
        tmp = wq;
    }

    exit_critical(imask);

    return 0;
}

int wakeup(struct wait_queue *wq)
{
    unsigned int imask = enter_critical();
        
    /* wake up all task waiting on the event q, let the task return proper error code */
    while(wq->task){
        wq->task->state = TASK_READY;
        add_to_ready_q(wq->task);
        wq->task->wait_queue = NULL;
        wq->task = wq->task->next; /* move to the next task in the queue */
   }
    exit_critical(imask);
    return 0;
}

void rose_event_thread()
{
    while(1) {


    }
}

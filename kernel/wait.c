/* Rose RT-Kernel
 * Copyright (C) 2018 Arul Bose<bose.arul@gmail.com>
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

static struct wait_queue *__sys_wait_list = NULL; /* Pointer to the list of tasks waiting on the wait queue */

int add_to_wait_queue(struct wait_queue *wq, int task_state)
{
    struct wait_queue *ride;

    unsigned int imask = enter_critical();
    
    __curr_running_task->state = task_state;
    __curr_running_task->wq = wq;
    remove_from_ready_q(__curr_running_task);
    __curr_running_task->next = NULL;
  
    wq->task = __curr_running_task;

    /* Add the waiting task to the sys wait queue */    
    if(!(__sys_wait_list)) {
        /* Empty; add the first node */
        __sys_wait_list = wq;
    }else{
        /* add wait queue to the tail of sytem wait list */
        ride = __sys_wait_list;
        while(ride->next){
            ride = ride->next;
        }
        ride->next = wq;
        wq->prev = ride;
    }

    exit_critical(imask);

    return 0;
}

int wakeup(struct wait_queue *wq)
{
    unsigned int imask = enter_critical();
    
    if(!__sys_wait_list){
        __early_printk("Invalid wakeup call\n");
        return -ENXIO;
    }

    if(__sys_wait_list == wq) {
        /* The first node is the task to be removed */
        __sys_wait_list = wq->next;
    }else{
        /* Cut and connect the chain */
        wq->prev->next = wq->next;
        wq->next->prev = wq->prev;
    }
    
    wq->task->state = TASK_READY;
    wq->task->wq = NULL; 
    add_to_ready_q(wq->task);

    exit_critical(imask);

    return 0;
}

/* Runs in the context of rose_event_thread()
 * Wake all the thread; will be called from event_group thread */

void __rose_wake()
{
    struct wait_queue *ride;

    if(!__sys_wait_list)
        return;
    
    ride = __sys_wait_list;

    /* Wake up all waiting task if TASK_INTERRUPTIBLE */
    while(ride)
    {
        if(ride->task->state == TASK_INTERRUPTIBLE) {
        /* Wake task */
            if(ride == __sys_wait_list) {
                /* Need to move the head */
                __sys_wait_list = ride->next;
                __sys_wait_list->prev = NULL;
            } else {
               
               ride->prev->next = ride->next;
               ride->next->prev = ride->prev; 
            }
            ride->task->state = TASK_READY;
            ride->task->wq = NULL;
            add_to_ready_q(ride->task);      
            ride = ride->next; /* move to the next wq */
        } 
    }
}

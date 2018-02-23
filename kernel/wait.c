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
static void __wait_timeout(struct wait_queue *wq, int timeout);
static void __wait_handler(void *ptr);

int __finish_wait(struct wait_queue *wq)
{
    int ret = 0;

    unsigned int imask = enter_critical();

    if(wq->task->timer != NULL) {
        /* Stop the timer and remove the timer from the list */
        remove_from_timer_list(wq->task->timer, &active_timer_head);
        wq->task->timer = NULL;    
    }
    if(wq->timeout == E_OS_TIMEOUT) {
        ret = E_OS_TIMEOUT;
    }
    wq->timeout = __TIMER_OFF;
    wq->task->wq = NULL;

    exit_critical(imask);

    return ret;

}

static void __wait_timeout()
{
    struct timer_list *timer = NULL;

    if (!(timer = create_timer(__wait_handler, __curr_running_task, timeout)){
        pr_panic("Timer creation failed\n");
    }
    __curr_running_task->timer = &timer;
    start_timer(&timer);
    __curr_running_task->timeout = __TIMER_ON;
}

static void __wait_handler(void *ptr)
{
    TCB *t = (TCB *)ptr;
    stop_timer(t->timer);
    delete_timer(t->timer);
    t->timeout = E_OS_TIMEOUT;
    t->timer = NULL;
}

int __add_to_wait_queue(struct wait_queue *wq, int task_state, int timeout)
{
    struct wait_queue *ride;
    TCB *t;

    unsigned int imask = enter_critical();

   /* Put the current task to sleep in its waitqueue */    
    __curr_running_task->state = task_state;
    __curr_running_task->wq = wq;
    remove_from_ready_q(__curr_running_task);
    __curr_running_task->next = NULL;
 
    /* Add the tasks in its wait_queue */ 
    if(!(wq->task)) {
        wq->task = __curr_running_task;
    }else{
        t = wq->task;
        while(t->next){
            t = t->next;
        }
        t->next = __curr_running_task;
    }
    /* Check if the waitqueue is already in the list */
    if(wq->next == NULL && wq->prev == NULL) {
        /* Now add the waitqueue to the sys wait queue */    
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
   }
    /* Wait timeout;Avoid starting the timer is already started */
    if((timeout > 0) && (__curr_running_task->timeout == __TIMER_OFF)) {
        __curr_running_task->timeout = timeout;
        __wait_timeout();
    }

    exit_critical(imask);

    return 0;
}

/* Wakeup all the task waiting on thge wait queue */
int wakeup(struct wait_queue *wq)
{
    unsigned int imask = enter_critical();
    
    TCB *t = wq->task;
    
    while(t) {
        t->state = TASK_READY;   
        add_to_ready_q(t);
        t = t->next;
    }    
    wq->task = NULL;

    exit_critical(imask);

    return 0;
}

/* Runs in the context of rose_event_thread()
 * Wake all the thread; will be called from event_group thread */

void __rose_wake()
{
    struct wait_queue *ride;
    struct wait_queue *wq;

    if(!__sys_wait_list)
        return;
    
    ride = __sys_wait_list;

    /* Wake up all waiting task if TASK_INTERRUPTIBLE */
    while(ride)
    {
        if(ride->task->state == TASK_INTERRUPTIBLE || ride->task->timeout == E_OS_TIMEOUT ) {
        /* Wake task */
            if(ride == __sys_wait_list) {
                /* Need to move the head */
                __sys_wait_list = ride->next;
                if(__sys_wait_list)
                    __sys_wait_list->prev = NULL;
            } else {
               /* Remove the task from the waitqueue */ 
               ride->prev->next = ride->next;
               ride->next->prev = ride->prev; 
            }
            ride->task->state = TASK_READY;
            add_to_ready_q(ride->task);
            wq = ride;
            wq->prev = wq->next = NULL;
            ride = ride->next; /* move to the next wq */
        } 
    }
}

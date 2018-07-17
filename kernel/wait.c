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

#include <RosX.h>

/* Rose Wait Q
 * */

static struct wait_queue *__rx_sys_wait_list = NULL; /* Pointer to the list of tasks waiting on the wait queue */
static void __rx_wait_timeout(struct timer_list *timer);
static void __rx_wait_handler(void *ptr);
static void rx_wake_task_in_the_queue(struct wait_queue **ride);

int __rx_finish_wait()
{
    int ret = 0;

    unsigned int imask = rx_enter_critical();
    /* Find the current task */

    if(__rx_curr_running_task->timer != NULL) {
        /* Stop the timer and remove the timer from the list */
        __rx_remove_from_timer_list(__rx_curr_running_task->timer, &__rx_active_timer_head);
        __rx_curr_running_task->timer = NULL;    
    }
    if(__rx_curr_running_task->timeout == E_OS_TIMEOUT) {
        ret = E_OS_TIMEOUT;
    }
    __rx_curr_running_task->timeout = __RX_TIMER_OFF;
    __rx_curr_running_task->wq = NULL;

    rx_exit_critical(imask);

    return ret;

}

static void __rx_wait_timeout(struct timer_list *timer)
{
    if ( OS_OK != rx_create_timer(timer, __rx_wait_handler, __rx_curr_running_task,  __rx_curr_running_task->timeout)){
        pr_panic("Timer creation failed\n");
    }
    __rx_curr_running_task->timer = timer;
    rx_start_timer(timer);
    __rx_curr_running_task->timeout = __RX_TIMER_ON;
}

/* Timer context */
static void __rx_wait_handler(void *ptr)
{
    RX_TASK *t = (RX_TASK *)ptr;
    t->timeout = E_OS_TIMEOUT;
    t->timer = NULL;
}

int __rx_add_to_wait_queue(struct wait_queue *wq, int task_state, int timeout, struct timer_list *timer)
{
    struct wait_queue *ride;
    RX_TASK *t;

    unsigned int imask = rx_enter_critical();

   /* Put the current task to sleep in its waitqueue */    
    __rx_curr_running_task->state = task_state;
    __rx_curr_running_task->wq = wq;
    __rx_remove_from_ready_q(__rx_curr_running_task);
    __rx_curr_running_task->next = NULL;
 
    /* Add the tasks in its wait_queue */ 
    if(!(wq->task)) {
        wq->task = __rx_curr_running_task;
    }else{
        t = wq->task;
        while(t->next){
            t = t->next;
        }
        t->next = __rx_curr_running_task;
    }
    /* Check if the waitqueue is already in the list */
        /* Now add the waitqueue to the sys wait queue */    
    if(!(__rx_sys_wait_list)) {
            /* Empty; add the first node */
            __rx_sys_wait_list = wq;
    }else{
        /* add wait queue to the tail of sytem wait list */
        ride = __rx_sys_wait_list;
        while(ride->next){
            if(ride == wq){
                break; /* wq is already in the list */
            }
            ride = ride->next;
        }
        if(!(ride == wq)) { /* wq is already in the list */
            ride->next = wq;
            wq->prev = ride;
        }
   }
    /* Wait timeout; Avoid starting the timer if already started */
    if((timeout > 0) && (__rx_curr_running_task->timeout == __RX_TIMER_OFF)) {
        __rx_curr_running_task->timeout = timeout;
        __rx_wait_timeout(timer);
    }

    rx_exit_critical(imask);

    return 0;
}

/* Wakeup all the task waiting on the wait queue */
int rx_wakeup(struct wait_queue *wq)
{
    RX_TASK *ready;
  
    unsigned int imask = rx_enter_critical();
    
    RX_TASK *t = wq->task;
    
    while(t) {
        t->state = RX_TASK_READY; 
        ready = t;  
        t = t->next;
        __rx_add_to_ready_q(ready);
    }    
    wq->task = NULL;
    /* Remove the queue from the sys queue list */
    if(wq == __rx_sys_wait_list) {
         __rx_sys_wait_list = wq->next;
         if(__rx_sys_wait_list) {
              __rx_sys_wait_list->prev = NULL;
         }
    }else{
         /* Remove the task from the waitqueue */
         wq->prev->next = wq->next;
         wq->next->prev = wq->prev;
         wq->prev = wq->next = NULL;     
    }

    rx_exit_critical(imask);

    return 0;
}

/* Wake all the task in this queue */
static void rx_wake_task_in_the_queue(struct wait_queue **ride)
{
    RX_TASK *tn;
    RX_TASK *tp;
    RX_TASK *ready;
    tn = tp = (*ride)->task;
    
    while(tn){
        if((tn->state == RX_TASK_INTERRUPTIBLE) || \
           (tn->timeout == E_OS_TIMEOUT) ) {
            /* Wake up task with state RX_TASK_INTERRUPTIBLE or E_OS_TIMEOUT */
            tn->state = RX_TASK_READY;
            ready = tn;
            if(tn == (*ride)->task){
                /* Move the head of the queue */
                tn = tn->next;
                (*ride)->task = tn;
                tp = tn;
                __rx_add_to_ready_q(ready);
           }else{
               /* Remove the task if in between the task chain */
               tn = tn->next;
               tp->next = tn;
               __rx_add_to_ready_q(ready);
           }
                    
       }else { /* !RX_TASK_INTERRUPTIBLE && !E_OS_TIMEOUT */
                    tp = tn;
                    tn = tn->next;
             }
    }

}

/* Runs in the context of rosx_event_thread()
 * Wake all the tasks in a queue list; Will be called from event_group thread */

void __rx_wake()
{
    struct wait_queue *ride;
    struct wait_queue *wq;

    if(!__rx_sys_wait_list)
        return;
    
    ride = __rx_sys_wait_list;
    /* Walk through each queue in the queue list and wake up all the task
     * if RX_TASK_INTERRUPTIBLE or E_OS_TIMEOUT
     */
    while(ride)
    {
        if(ride == __rx_sys_wait_list) {
            rx_wake_task_in_the_queue(&ride);  
            if(ride->task == NULL) {
                 wq = ride;
                /* If the task list is empty for the queue the move the head to the next one  */
                __rx_sys_wait_list = ride->next;
                 wq->prev = wq->next = NULL;
                if(__rx_sys_wait_list){
                    __rx_sys_wait_list->prev = NULL;
                }
            }
        }else{ /* !(ride == __rx_sys_wait_list) */
             rx_wake_task_in_the_queue(&ride);  
             if(ride->task == NULL) {
                 wq = ride;
                 /* Remove the task from the waitqueue */ 
                 ride->prev->next = ride->next;
                 ride->next->prev = ride->prev; 
                 wq->prev = wq->next = NULL;
             }
       }
       ride = ride->next; /* move to the next wq */
    }
}

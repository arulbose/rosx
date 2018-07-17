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

#include <RosX.h>

void __rx_sem_handler(void *ptr);


/* Runtime creation of semaphore */
int rx_create_semaphore(struct semaphore *sem, int val)
{
    unsigned int imask;

    if(val <=0 ){
		pr_error( "invalid semaphore init value\n");
		return OS_ERR;
    }

    imask = rx_enter_critical();

    sem->init_val = val; 
    sem->curr_val = val; 
    sem->task = NULL;

    rx_exit_critical(imask);
    return OS_OK;
}

/* Runtime deletion of semaphore */
void rx_delete_semaphore(struct semaphore *p)
{
    RX_TASK *ready;
    unsigned int imask = rx_enter_critical();

    /* wake up all task waiting on the sem q, let the task return proper error code */
    while(p->task){
       p->task->state = RX_TASK_READY;
       p->task->sem = NULL;
       /* Make sure if the task is waiting for the timeout on the semaphore list */
       if(p->task->timer) {
           __rx_remove_from_timer_list(p->task->timer, &__rx_active_timer_head);
           p->task->timer = NULL;
       }
            ready = p->task;
            p->task = p->task->next; /* move to the next task in the queue */
            __rx_add_to_ready_q(ready);
    }

    rx_exit_critical(imask);

    return;
}

/* increment sem value */
int rx_semaphore_post(struct semaphore *sem)
{
	RX_TASK *t;

	unsigned int imask = rx_enter_critical();
	/* Wake up the task sleeping in the it's queue */
	if(sem->task) {
		t = sem->task;
		sem->task = sem->task->next; /* move the next task in the sem wait queue */
		t->state = RX_TASK_READY;
		t->sem = NULL;
		__rx_add_to_ready_q(t);
		sem->curr_val ++; /* inc the count as one task is woke up from the wait queue */
	}

	rx_exit_critical(imask);
	
	return OS_OK;
}

/* Running in timer contex */
void __rx_sem_handler(void *ptr)
{
    RX_TASK *lead;
    RX_TASK *follow;
    RX_TASK *task = (RX_TASK *)ptr;
    struct semaphore *sem = task->sem;

    __rx_early_printk("timer handler start %s\n", task->name);
    if(task->sem == NULL) {
        /* Already acquired the semaphore and clean-up done in the sem_post() */
        return;
     }
     /* remove the task from sem wait queue<start> */
     lead = follow = sem->task;
     if(lead == task){ /* if current running task is actually the first task in the wait queue */
         sem->task = sem->task->next;
     }else{

         while(lead != task) {
             follow = lead;
             lead = lead->next;
         }
             follow->next = lead->next;
     }
    sem->curr_val ++; /* Increment the sem val as a task has timed out and no more waiting on the sem queue */

    /* Clean-up timer and notify timeout */
    task->timer = NULL;
    task->timeout = E_OS_TIMEOUT;
    /* Set the task as runnable */
    task->state = RX_TASK_READY;
    __rx_add_to_ready_q(task);

    /* remove the task from sem wait queue<finish> */
    task->sem = NULL;
    __rx_early_printk("timer handler end %s\n", task->name);
}

/* Task waiting on semaphore with timeout will self suspend and or waked
 *  up either by timer handler or by semaphore post function 
 */
static int __rx_sem_timeout(struct semaphore *p, int timeout)
{
    struct timer_list timer;

    rx_init_timer(&timer, __rx_sem_handler, __rx_curr_running_task, timeout);
    rx_start_timer(&timer);
    __rx_curr_running_task->timeout = __RX_TIMER_ON;
    __rx_curr_running_task->timer = &timer;
    rx_suspend_task(MYSELF);

   if(E_OS_TIMEOUT == __rx_curr_running_task->timeout) {
        return E_OS_TIMEOUT;
    }else{
        return OS_OK; /* Already task has acquired the sem; clean-up done by the sem_post() */
    }
}

/* Decrement semaphore value and if less than 0 
 *  add the task in wait queue  
 */
int rx_semaphore_wait(struct semaphore *sem, int timeout)
{
    RX_TASK *ride;
	
    unsigned int imask = rx_enter_critical();
    /* decrement the semaphore; the value in negative specify 
           number of task waiting on the semaphore queue */
    sem->curr_val --;
    if(sem->curr_val >= 0) {
       rx_exit_critical(imask);
       return OS_OK; /* return immediately for all positive values */
    }

    if(timeout == OS_NO_WAIT) {
        sem->curr_val ++; /* if not ready to wait */
        rx_exit_critical(imask);
        return E_OS_UNAVAIL;
    }
    /* Remove the task from the ready queue */
     __rx_curr_running_task->state = RX_TASK_SUSPEND;
     __rx_curr_running_task->sem = sem;
     __rx_curr_running_task->timeout = __RX_TIMER_OFF;
     __rx_remove_from_ready_q(__rx_curr_running_task);
     __rx_curr_running_task->next = NULL;

    /* Add the task to the sem sleep queue */	
    if(!(sem->task)) {
       sem->task = __rx_curr_running_task;
    }else{
        ride = sem->task;
	    while(ride->next){
	        ride = ride->next;
            }
	    ride->next = __rx_curr_running_task;
    }
    /* Check if there is a timeout for semaphore <start> */
    if(timeout > 0) {
        rx_exit_critical(imask);
        return __rx_sem_timeout(sem, timeout);
    }

    rx_exit_critical(imask);
    rx_sched();

    if(!sem){
	return E_OS_UNAVAIL; /* semaphore is deleted and ask waken-up */
    }else{
        return OS_OK;
    }
}

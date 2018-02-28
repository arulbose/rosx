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

void __sem_handler(void *ptr);


#if(CONFIG_SEMAPHORE_COUNT > 0)
/* Runtime creation of semaphore */
struct semaphore * create_semaphore(int val)
{
    struct semaphore *sem = NULL;
    unsigned int imask;

    if(val <=0 ){
		pr_error( "invalid semaphore init value\n");
		return NULL;
    }

    imask = enter_critical();

    if(NULL == (sem = __alloc_pool(SEMAPHORE_POOL))) {
        pr_error( "create_semaphore failed\n");
	exit_critical(imask);
        return NULL;
    }
    sem->init_val = val; 
    sem->curr_val = val; 
    sem->task = NULL;

    exit_critical(imask);
    return sem;
}

/* Runtime deletion of semaphore */
void delete_semaphore(struct semaphore *p)
{
    TCB *ready;
    unsigned int imask = enter_critical();

    /* wake up all task waiting on the sem q, let the task return proper error code */
    while(p->task){
       p->task->state = TASK_READY;
       p->task->sem = NULL;
       /* Make sure if the task is waiting for the timeout on the semaphore list */
       if(p->task->timer) {
           __remove_from_timer_list(p->task->timer, &__active_timer_head);
           p->task->timer = NULL;
       }
            ready = p->task;
            p->task = p->task->next; /* move to the next task in the queue */
            __add_to_ready_q(ready);
    }

    __free_pool(p, SEMAPHORE_POOL);

    exit_critical(imask);

    return;
}
#endif

/* increment sem value */
int semaphore_post(struct semaphore *sem)
{
	TCB *t;

	unsigned int imask = enter_critical();
	/* Wake up the task sleeping in the it's queue */
	if(sem->task) {
		t = sem->task;
		sem->task = sem->task->next; /* move the next task in the sem wait queue */
		t->state = TASK_READY;
		t->sem = NULL;
		__add_to_ready_q(t);
		sem->curr_val ++; /* inc the count as one task is woke up from the wait queue */
	}

	exit_critical(imask);
	
	return OS_OK;
}

/* Running in timer contex */
void __sem_handler(void *ptr)
{
    TCB *lead;
    TCB *follow;
    TCB *task = (TCB *)ptr;
    struct semaphore *sem = task->sem;

    __early_printk("timer handler start %s\n", task->name);
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
    task->state = TASK_READY;
    __add_to_ready_q(task);

    /* remove the task from sem wait queue<finish> */
    task->sem = NULL;
    __early_printk("timer handler end %s\n", task->name);
}

/* Task waiting on semaphore with timeout will self suspend and or waked
 *  up either by timer handler or by semaphore post function 
 */
static int __sem_timeout(struct semaphore *p, int timeout)
{
    struct timer_list timer;

    init_timer(&timer, __sem_handler, __curr_running_task, timeout);
    start_timer(&timer);
    __curr_running_task->timeout = __TIMER_ON;
    __curr_running_task->timer = &timer;
    suspend_task(MYSELF);

   if(E_OS_TIMEOUT == __curr_running_task->timeout) {
        return E_OS_TIMEOUT;
    }else{
        return OS_OK; /* Already task has acquired the sem; clean-up done by the sem_post() */
    }
}

/* Decrement semaphore value and if less than 0 
 *  add the task in wait queue  
 */
int semaphore_wait(struct semaphore *sem, int timeout)
{
    TCB *ride;
	
    unsigned int imask = enter_critical();
    /* decrement the semaphore; the value in negative specify 
           number of task waiting on the semaphore queue */
    sem->curr_val --;
    if(sem->curr_val >= 0) {
       exit_critical(imask);
       return OS_OK; /* return immediately for all positive values */
    }

    if(timeout == OS_NO_WAIT) {
        sem->curr_val ++; /* if not ready to wait */
        exit_critical(imask);
        return E_OS_UNAVAIL;
    }
    /* Remove the task from the ready queue */
     __curr_running_task->state = TASK_SUSPEND;
     __curr_running_task->sem = sem;
     __curr_running_task->timeout = __TIMER_OFF;
     __remove_from_ready_q(__curr_running_task);
     __curr_running_task->next = NULL;

    /* Add the task to the sem sleep queue */	
    if(!(sem->task)) {
       sem->task = __curr_running_task;
    }else{
        ride = sem->task;
	    while(ride->next){
	        ride = ride->next;
            }
	    ride->next = __curr_running_task;
    }
    /* Check if there is a timeout for semaphore <start> */
    if(timeout > 0) {
        exit_critical(imask);
        return __sem_timeout(sem, timeout);
    }

    exit_critical(imask);
    rose_sched();

    if(!sem){
	return E_OS_UNAVAIL; /* semaphore is deleted and ask waken-up */
    }else{
        return OS_OK;
    }
}

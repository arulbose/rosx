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
/* how to de-init mutex created in static */
static int __mutex_timeout(struct mutex *p, unsigned int timeout);
void __mutex_handler(void *ptr);

/* for static allocation */
void init_mutex(struct mutex *mutex)
{
    mutex->lock = __UNLOCKED;
    mutex->owner = NULL;
    mutex->next = NULL;
    mutex->task = NULL;
}

#if(CONFIG_MUTEX_COUNT > 0)
/* dynamic creation of mutex */
struct mutex * create_mutex()
{
	struct mutex *mutex = NULL;

	unsigned int imask = enter_critical();

	if(NULL == (mutex = __alloc_pool(MUTEX_POOL))) {
		pr_error( " create_mutex failed\n");
		exit_critical(imask);
		return NULL;
	}		
	
	mutex->lock = __UNLOCKED;
	mutex->owner = NULL;
	mutex->next = NULL;
	mutex->task = NULL;

	exit_critical(imask);

	return mutex;	
}

/* Runtime delete of mutex */
void delete_mutex(struct mutex *p)
{
	unsigned int imask = enter_critical();

	/* wake up all task waiting on the mutext q, let the task return proper error code */	
	while(p->task){
	    p->task->state = TASK_READY;
	    p->task->mutex = NULL;
	    /* Make sure if the task is waiting for the timeout on the mutex list */
            if(p->task->timer) {
                remove_from_timer_list(p->task->timer, &active_timer_head);
                p->task->timer = NULL;
            }
	    add_to_ready_q(p->task);
	    p->task = p->task->next; /* move to the next task in the queue */
	}

	p->owner = NULL;

	__free_pool(p, MUTEX_POOL);

	exit_critical(imask);
}

#endif
/* unlock and wake up if any task pending 
 * supports priority inheritance
*/
int mutex_unlock(struct mutex *p)
{
	TCB *t;

	unsigned imask = enter_critical();

        if(p->lock == __LOCKED){
	    if(p->owner != __curr_running_task){ /* Check for mutex ownership */
		pr_error("Illegal mutex unlock %s %s\n", p->owner->name, __curr_running_task->name);
	        exit_critical(imask);
	        return OS_ERR;
	    }
            p->lock = __UNLOCKED;

#ifdef CONFIG_PRIO_INHERITANCE
		if(p->owner->prio != p->owner->orig_prio) {
		    p->owner->prio = p->owner->orig_prio;
		    /* Back to its original priority */
		    remove_from_ready_q(p->owner);
		    add_to_ready_q(__curr_running_task);
		}
#endif
	    t = p->task;

	    /* Wake up the next task waiting on the mutex */
	    if(t) {
	    		p->task = p->task->next; /* move to the next task waiting in the queue */
			 /* Make sure if the task is waiting for the timeout on the mutex list */
			 if(t->timer) {
               			remove_from_timer_list(t->timer, &active_timer_head);
               			t->timer = NULL;
            		}
			/* give the lock to the next task waiting in the queue */
            		p->owner = t;
#ifdef CONFIG_PRIO_INHERITANCE
			/* Make the owner to inherit the max priority of task waiting on the mutex queue */
			t = p->task;
			while(t) 
			{
				if(p->owner->prio > t->prio) {
					p->owner->prio = t->prio;
				}
				t = t->next;		
			}
#endif
			p->owner->state = TASK_READY;
			p->owner->mutex = NULL;
			p->lock = __LOCKED;
			add_to_ready_q(p->owner);
	    }
       } 

    exit_critical(imask);
    return OS_OK; 
}

/* Callback to remove mutex waiting in the mutex queue when timer expired */
void __mutex_handler(void *ptr)
{
    TCB *task = (TCB *)ptr;
    struct mutex *mutex = task->mutex;
    TCB *t1;
    TCB *t2;

    if(mutex->owner == task)
	return; /* Already task has acquired the mutex; clean-up done by the mutex_unlock() */

  /* Task failed to acquired mutex hence remove the task from mutex wait queue<start> */
    t1 = t2 = mutex->task;
    if(t1 == task){
          mutex->task = mutex->task->next;
    }else{
    /*  */
        while(t1 != task) {
            t2 = t1;
            t1 = t1->next;
        }
        t2->next = t1->next;
    }
#ifdef CONFIG_PRIO_INHERITANCE
    /* Check if the outgoing thread prio is inherited by the current owner */
    if(mutex->owner->prio == task->prio) {
        mutex->owner->prio = mutex->owner->orig_prio; /* restore to the original prio */

        /* Make the owner to inherit the max priority of task waiting on the mutex queue */
        t1 = mutex->task;
        while(t1)
        {
            if(mutex->owner->prio > t1->prio) {
                mutex->owner->prio = t1->prio;
            }
            t1 = t1->next;
        }
    }

   if(mutex->owner->state == TASK_READY) {
        remove_from_ready_q(mutex->owner);
        add_to_ready_q(mutex->owner);    /* rearrange the prio of the owner */
    }
#endif

    /* Clean-up timer resources acquired of the timer expired task*/
    task->timer = NULL;
    task->mutex = NULL;
    /* Put the expired timer task in to ready queue */
    task->state = TASK_READY;
    add_to_ready_q(task);
}

/* Waiting on the timeout queue if mutex has also timeout */
static int __mutex_timeout(struct mutex *p, unsigned int timeout)
{
    struct timer_list timer;

    init_timer(&timer, __mutex_handler, __curr_running_task, msecs_to_ticks(timeout));
    start_timer(&timer);
    __curr_running_task->timer = &timer; 
    suspend_task(MYSELF);
    
   if(p->owner == __curr_running_task)
        return OS_OK; /* Already task has acquired the mutex; clean-up done by the mutex_unlock() */
    else
        return E_OS_TIMEOUT;
}

/* Task sleep if lock not available; add to timer list if time out is greater than 0 */
int mutex_lock(struct mutex *p, unsigned int timeout)
{
	TCB *t1;

	unsigned int imask = enter_critical();

	if(p->lock == __UNLOCKED) { /* If lock is available? */
	    p->lock = __LOCKED;
	    p->owner = __curr_running_task;
	    exit_critical(imask);
	    return OS_OK; /* lock is available return immediately */
	}

	if(timeout == OS_NO_WAIT) { /* if no wait; return immediately */
	    exit_critical(imask);
	    return E_OS_UNAVAIL;
	}
	pr_info("entering mutex_lock %s\n", __curr_running_task->name);
	/* Put the task to sleep in the mutex wait list */
	__curr_running_task->state = TASK_SUSPEND;
	__curr_running_task->mutex = p;
	remove_from_ready_q(__curr_running_task);
	__curr_running_task->next = NULL;

        /* Add the task to the mutex sleep queue */
	if(!(p->task)) {
	   p->task = __curr_running_task;
	}else{
	   t1 = p->task;
	   /* Add the new task to the end of the queue */
	   while(t1->next)
		t1 = t1->next;

	    t1->next = __curr_running_task;
	}

	/* Avoid priority inversion by changing the prio to the task which has the max priority waiting in the mutex wait queue */
#ifdef CONFIG_PRIO_INHERITANCE
	if(p->owner->prio > __curr_running_task->prio) {
	    p->owner->prio = __curr_running_task->prio;  /* Let the owner inherit the prio of the task */
	    if(p->owner->state == TASK_READY) { /* Re-arrange the task in the ready queue */
                 remove_from_ready_q(p->owner);
		 add_to_ready_q(p->owner);
	    }
	}
#endif
	if(timeout > 0) {
		exit_critical(imask); /* __mutex_timeout will sleep hence exit critical */
		return __mutex_timeout(p, timeout);
	}
	exit_critical(imask);

	rose_sched();
	/* In case mutex is deleted while the process is waiting for mutex */
	if(!p)
		return E_OS_UNAVAIL;
	else
		return OS_OK; 
}

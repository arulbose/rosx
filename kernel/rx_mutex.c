/* Rosx RT-Kernel
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
/* how to de-init mutex created in static */
static int __rx_mutex_timeout(struct mutex *p, unsigned int timeout);
void __rx_mutex_handler(void *ptr);

/* Runtime creation of mutex */
int rx_create_mutex(struct mutex *m)
{

	unsigned int imask = rx_enter_critical();

	m->lock = __RX_MUTEX_UNLOCKED;
	m->owner = NULL;
	m->next = NULL;
	m->task = NULL;

	rx_exit_critical(imask);

	return OS_OK;	
}

/* Runtime deletion of mutex */
void rx_delete_mutex(struct mutex *m)
{
        RX_TASK *ready;
	unsigned int imask = rx_enter_critical();

	/* wake up all task waiting on the mutext q, let the task return proper error code */	
	while(m->task){
	    m->task->state = RX_TASK_READY;
	    m->task->mutex = NULL;
	    /* Make sure if the task is waiting for the timeout on the mutex list */
            if(m->task->timer) {
                rx_stop_timer(m->task->timer);
                m->task->timer = NULL;
            }
            ready = m->task;
	    m->task = m->task->next; /* move to the next task in the queue */
	    __rx_add_to_ready_q(ready);
	}

	m->owner = NULL;

	rx_exit_critical(imask);
}

/* unlock and wake up if any task pending 
 * supports priority inheritance
*/
int rx_mutex_unlock(struct mutex *p)
{
	RX_TASK *t;

	unsigned imask = rx_enter_critical();

        if(p->lock == __RX_MUTEX_LOCKED){
	    if(p->owner != __rx_curr_running_task){ /* Check for mutex ownership */
		pr_error("Illegal mutex unlock %s %s\n", p->owner->name, __rx_curr_running_task->name);
	        rx_exit_critical(imask);
	        return OS_ERR;
	    }
            p->lock = __RX_MUTEX_UNLOCKED;

#ifdef CONFIG_PRIO_INHERITANCE
		if(p->owner->prio != p->owner->orig_prio) {
		    p->owner->prio = p->owner->orig_prio;
                    pr_dbg("Mutex owner %s back to original prio %d\n", p->owner->name, p->owner->prio);
		    /* Back to its original priority */
		    __rx_remove_from_ready_q(p->owner);
		    __rx_add_to_ready_q(__rx_curr_running_task);
		}
#endif
	    t = p->task;

	    /* Wake up the next task waiting on the mutex */
	    if(t) {
	    		p->task = p->task->next; /* move to the next task waiting in the queue */
			 /* Make sure if the task is waiting for the timeout on the mutex list */
			 if(t->timer) {
               			rx_stop_timer(t->timer);
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
			p->owner->state = RX_TASK_READY;
			p->owner->mutex = NULL;
			p->lock = __RX_MUTEX_LOCKED;
			__rx_add_to_ready_q(p->owner);
	    }
       } 

    rx_exit_critical(imask);
    return OS_OK; 
}

/* Callback to remove mutex waiting in the mutex queue when timer expired */
void __rx_mutex_handler(void *ptr)
{
    RX_TASK *task = (RX_TASK *)ptr;
    struct mutex *mutex = task->mutex;
    RX_TASK *t1;
    RX_TASK *t2;

    if(mutex->owner == task)
	return; /* Already task has acquired the mutex; clean-up done by the rx_mutex_unlock() */

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

   if(mutex->owner->state == RX_TASK_READY) {
        __rx_remove_from_ready_q(mutex->owner);
        __rx_add_to_ready_q(mutex->owner);    /* rearrange the prio of the owner */
    }
#endif

    /* Clean-up timer resources acquired of the timer expired task*/
    task->timer = NULL;
    task->mutex = NULL;
    /* Put the expired timer task in to ready queue */
    task->state = RX_TASK_READY;
    __rx_add_to_ready_q(task);
}

/* Waiting on the timeout queue if mutex has also timeout */
static int __rx_mutex_timeout(struct mutex *p, unsigned int timeout)
{
    struct timer_list timer;

    rx_init_timer(&timer, __rx_mutex_handler, __rx_curr_running_task, timeout);
    rx_start_timer(&timer);
    __rx_curr_running_task->timer = &timer; 
    rx_suspend_task(MYSELF);
    
   if(p->owner == __rx_curr_running_task)
        return OS_OK; /* Already task has acquired the mutex; clean-up done by the rx_mutex_unlock() */
    else
        return E_OS_TIMEOUT;
}

/* Task sleep if lock not available; add to timer list if time out is greater than 0 */
int rx_mutex_lock(struct mutex *p, int timeout)
{
	RX_TASK *t;

	unsigned int imask = rx_enter_critical();

	if(p->lock == __RX_MUTEX_UNLOCKED) { /* If lock is available? */
	    p->lock = __RX_MUTEX_LOCKED;
	    p->owner = __rx_curr_running_task;
	    rx_exit_critical(imask);
	    return OS_OK; /* lock is available return immediately */
	}

	if(timeout == OS_NO_WAIT) { /* if no wait; return immediately */
	    rx_exit_critical(imask);
	    return E_OS_UNAVAIL;
	}
	pr_dbg("entering rx_mutex_lock %s\n", __rx_curr_running_task->name);
	/* Put the task to sleep in the mutex wait list */
	__rx_curr_running_task->state = RX_TASK_SUSPEND;
	__rx_curr_running_task->mutex = p;
	__rx_remove_from_ready_q(__rx_curr_running_task);
	__rx_curr_running_task->next = NULL;

        /* Add the task to the mutex sleep queue */
	if(!(p->task)) {
	   p->task = __rx_curr_running_task;
	}else{
	   t = p->task;
	   /* Add the new task to the end of the queue */
	   while(t->next)
		t = t->next;

	    t->next = __rx_curr_running_task;
	}

	/* Avoid priority inversion by changing the prio to the task which has the max priority waiting in the mutex wait queue */
#ifdef CONFIG_PRIO_INHERITANCE
	if(p->owner->prio > __rx_curr_running_task->prio) {
	    p->owner->prio = __rx_curr_running_task->prio;  /* Let the owner inherit the prio of the task */
            pr_dbg("Mutex owner %s new prio %d\n", p->owner->name, p->owner->prio);
	    if(p->owner->state == RX_TASK_READY) { /* Re-arrange the task in the ready queue */
                 __rx_remove_from_ready_q(p->owner);
		 __rx_add_to_ready_q(p->owner);
	    }
	}
#endif
	if(timeout > 0) {
		rx_exit_critical(imask); /* __mutex_timeout will sleep hence exit critical */
		return __rx_mutex_timeout(p, timeout);
	}
	rx_exit_critical(imask);

	rx_sched();
	/* In case mutex is deleted while the process is waiting for mutex */
	if(!p)
		return E_OS_UNAVAIL;
	else
		return OS_OK; 
}

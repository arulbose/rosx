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

/* struct to pack the events in the event receive queue */
struct event {
    void *p;
    unsigned int flag;
};

static struct queue *event_recv_q = NULL; /* queue to hold the events received */
static int process_event(struct event_group *, unsigned int flag);

/* Rose Event Group
 * */
/* A event group created can be shared with multiple thread for synchronization */

/* Create dynamic event create */

#if(CONFIG_EVENT_COUNT > 0)
/* Runtime creation of event group */
struct event_group * create_event_group()
{
	struct event_group *event = NULL;

	unsigned int imask = enter_critical();

        if(NULL == (event = __alloc_pool(EVENT_POOL))) {
           pr_error( " create_event failed\n");
           return NULL;
        }

	event->task = NULL;
	event->next = NULL;

	exit_critical(imask);

	return event;
}

/* Runtime deletion of event group. Notify the task waiting for the events */
void delete_event_group(struct event_group *p)
{
	unsigned int imask = enter_critical();

        /* wake up all task waiting on the event q, let the task return proper error code */
        while(p->task){
            p->task->state = TASK_READY;
            add_to_ready_q(p->task);
            p->task->event = NULL;
            p->task = p->task->next; /* move to the next task in the queue */
        }

	__free_pool(p, EVENT_POOL);
        exit_critical(imask);
}
#endif

/* Add events to the task */
int set_event_flag(unsigned int flag)
{
	__curr_running_task->event_flag |= flag;
	return OS_OK; 
}

int clear_event_flag(unsigned int flag)
{
	__curr_running_task->event_flag &= ~flag;
	__curr_running_task->event_recv &= ~flag;

	return OS_OK;
}

/* Task removed from ready queue and added in wait queue; task will be waked-up by notify_event(). 
 * In case of event occured when the task is not waiting on the event queue, event will miss;
 */
int wait_event_group(struct event_group *p, int flag)
{
	TCB *tmp;

	unsigned int imask = enter_critical();

	__curr_running_task->event_flag |= flag;
	/* Add the task to the event queue */
	/* add to the event group list */

	__curr_running_task->state = TASK_SUSPEND;
	__curr_running_task->event = p;
	remove_from_ready_q(__curr_running_task);
	__curr_running_task->next = NULL;

	  if(!(p->task)) {
                p->task = __curr_running_task;

            }else{
                tmp = p->task;
                while(tmp)
                    tmp = tmp->next;

                tmp = __curr_running_task;
            }
	
	exit_critical(imask);

	rose_sched();	

	return __curr_running_task->event_recv;
}

/* Event thread which process all system events; thread part of system threads with prio 0 */
void rose_event_thread()
{
    struct event e; 

    if(NULL == (event_recv_q = create_queue("event_q", sizeof(struct event), 8, Q_BLOCK|Q_CYCLIC_FULL))) {
		pr_panic("create_queue failed in rose_event_thread\n");
    }
	
    /* empty the queue when there is an event */
    while(1) {

       read_from_queue(event_recv_q, (char *)&e, sizeof(struct event), OS_NO_WAIT);
       process_event((struct event_group *)e.p, e.flag);		
    }

}

/* Write to event queue to be processed later; called also from interrupt */
void notify_event(struct event_group *eg, unsigned int flag)
{
     struct event e;

     e.p = eg;
     e.flag = flag;
     write_to_queue(event_recv_q, (char *)&e, sizeof(struct event)); 

     return;
}

/* Wake up all task waiting for a particular event/s  */
static int process_event(struct event_group *head, unsigned int flag)
{
	TCB *start = NULL;
	TCB *prev = NULL;


	unsigned int imask = enter_critical();
	
        /* Wake thread sleeping on the wait queues */
        __rose_wake();

	if(head == NULL) {
		pr_error( "In notify_event: no event_group to notify\n");
		exit_critical(imask);
		return OS_ERR;
	}

	start = prev = head->task;
	while(start != NULL) {
		
		if((start->event_flag & flag) != 0) {
			 start->event_recv = flag;
			 start->state = TASK_READY;
			 start->event = NULL;
			 start->next = NULL;
			/* Remove from the event group list */
			  if(start == head->task){
                                head->task = start->next;
				add_to_ready_q(start);
					if(head->task == NULL)
						break; /* no more node to parse */
				prev = head->task;
				start = head->task->next;
                          }else{
				prev->next = start->next;
				add_to_ready_q(start);
				start = prev->next;
                          }

		} else {
			prev = start;
			start = start->next; /* Move to the next node */
		}

	 }
	exit_critical(imask);
	return OS_OK;
}

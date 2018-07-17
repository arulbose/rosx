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

#include <RosX.h>

struct msg_queue rx_event_recv_q;
char rx_event_recv_q_start[sizeof(struct msg_queue) * CONFIG_SYS_EVENT_QUEUE_BLOCKS];

/* struct to pack the events in the event receive queue */
struct event {
    void *p;
    unsigned int flag;
};

static int rx_process_event(struct event_group *, unsigned int flag);

/* Rose Event Group
 * */
/* A event group created can be shared with multiple thread for synchronization */

/* Create dynamic event create */

/* Runtime creation of event group */
int rx_create_event_group(struct event_group *event)
{
	unsigned int imask = rx_enter_critical();

	event->task = NULL;
	event->next = NULL;

	rx_exit_critical(imask);

	return OS_OK;
}

/* Runtime deletion of event group. Notify the task waiting for the events */
void rx_delete_event_group(struct event_group *p)
{
        RX_TASK *ready;
	unsigned int imask = rx_enter_critical();

        /* wake up all task waiting on the event q, let the task return proper error code */
        while(p->task){
            p->task->state = RX_TASK_READY;
            p->task->event = NULL;
            ready = p->task;
            p->task = p->task->next; /* move to the next task in the queue */
            __rx_add_to_ready_q(ready);
        }

        rx_exit_critical(imask);
}

/* Add events to the task */
int rx_set_event_flag(unsigned int flag)
{
	__rx_curr_running_task->event_flag |= flag;
	return OS_OK; 
}

int rx_clear_event_flag(unsigned int flag)
{
	__rx_curr_running_task->event_flag &= ~flag;
	__rx_curr_running_task->event_recv &= ~flag;

	return OS_OK;
}

/* Task removed from ready queue and added in wait queue; task will be waked-up by notify_event(). 
 * In case of event occured when the task is not waiting on the event queue, event will miss;
 */
int rx_wait_event_group(struct event_group *p, int flag)
{
	RX_TASK *tmp;

	unsigned int imask = rx_enter_critical();

	__rx_curr_running_task->event_flag |= flag;
	/* Add the task to the event queue */
	/* add to the event group list */

	__rx_curr_running_task->state = RX_TASK_SUSPEND;
	__rx_curr_running_task->event = p;
	__rx_remove_from_ready_q(__rx_curr_running_task);
	__rx_curr_running_task->next = NULL;

	  if(!(p->task)) {
                p->task = __rx_curr_running_task;

            }else{
                tmp = p->task;
                while(tmp)
                    tmp = tmp->next;

                tmp = __rx_curr_running_task;
            }
	
	rx_exit_critical(imask);

	rx_sched();	

	return __rx_curr_running_task->event_recv;
}

/* Event thread which process all system events; thread part of system threads with prio 0 */
void rx_event_thread()
{
    struct event e; 

    if(OS_OK != rx_create_queue(&rx_event_recv_q, "event_q", sizeof(struct event), CONFIG_SYS_EVENT_QUEUE_BLOCKS, rx_event_recv_q_start, RX_Q_BLOCK|RX_Q_CYCLIC_FULL)) {
		pr_panic("create_queue failed in rosx_event_thread\n");
    }
	
    /* empty the queue when there is an event */
    while(1) {

       rx_read_from_queue(&rx_event_recv_q, (char *)&e, sizeof(struct event), OS_NO_WAIT);
       rx_process_event((struct event_group *)e.p, e.flag);		
    }

}

/* Write to event queue to be processed later; called also from interrupt */
void rx_notify_event(struct event_group *eg, unsigned int flag)
{
     struct event e;

     e.p = eg;
     e.flag = flag;
     rx_write_to_queue(&rx_event_recv_q, (const char *)&e, sizeof(struct event), OS_WAIT_FOREVER); 

     return;
}

/* Wake up all task waiting for a particular event/s  */
static int rx_process_event(struct event_group *head, unsigned int flag)
{
	RX_TASK *start = NULL;
	RX_TASK *prev = NULL;


	unsigned int imask = rx_enter_critical();
	
        /* Wake thread sleeping on the wait queues */
        __rx_wake();

	if(head == NULL) {
		pr_error( "In notify_event: no event_group to notify\n");
		rx_exit_critical(imask);
		return OS_ERR;
	}

	start = prev = head->task;
	while(start != NULL) {
		
		if((start->event_flag & flag) != 0) {
			 start->event_recv = flag;
			 start->state = RX_TASK_READY;
			 start->event = NULL;
			 start->next = NULL;
			/* Remove from the event group list */
			  if(start == head->task){
                                head->task = start->next;
				__rx_add_to_ready_q(start);
					if(head->task == NULL)
						break; /* no more node to parse */
				prev = head->task;
				start = head->task->next;
                          }else{
				prev->next = start->next;
				__rx_add_to_ready_q(start);
				start = prev->next;
                          }

		} else {
			prev = start;
			start = start->next; /* Move to the next node */
		}

	 }
	rx_exit_critical(imask);
	return OS_OK;
}

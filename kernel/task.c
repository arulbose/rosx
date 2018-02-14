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

/** create_task() - Create new task
 *  @tcb:		Task Control Block
 *  @task_name:		Name for the created task
 *  @prio:		Task priority
 *  @stack_ptr:		Stack pointer for the task. Can be NULL if CONFIG_STACK_ALLOC_DYNAMIC defined 
 *  @stack_size:	Size of the stack for the task
 *  @func:		Task entry function
 *  task_state:		Task state when created
 *  time_slice:		The time in ticks the process should run in case of task competing with same priority
 */
int create_task(TCB *tcb, char *task_name, int prio, void *stack_ptr, int stack_size, void (*func)(void), int task_state, int time_slice)
{
	TCB *tmp;
	unsigned int imask;

	if(strlen(task_name) > TASK_STR_LEN){
		pr_error( "%s %s\n", "Task name overflow", task_name);
		return OS_ERR;
	}

	imask = enter_critical();

	#ifdef CONFIG_STACK_ALLOC_DYNAMIC
	/* if stack_ptr is NULL go for dynamic stack allocation */
	if(!stack_ptr) {

  		if(((char *)__stack_start_ptr - CONFIG_SYSTEM_STACK_SIZE) > ((char *)__curr_stack_ptr - stack_size)){
	        	pr_error( "%s %s\n", "Stack size overflow\n", task_name);
			exit_critical(imask);
			return OS_ERR;
		}
		stack_ptr = __curr_stack_ptr;
		__curr_stack_ptr = ((char *)__curr_stack_ptr - (stack_size + 4));
	}
 	#endif

	strcpy((char *)tcb->name, task_name);	
	tcb->curr_stack_ptr = stack_ptr;
	tcb->stack_start_ptr = stack_ptr;
        if (prio > LEAST_PRIO)
            prio = LEAST_PRIO; /* Fix it to the LEAST_PRIO */

	tcb->prio = prio;
#ifdef CONFIG_PRIO_INHERITANCE
	tcb->orig_prio = prio;
#endif
	tcb->stack_size = stack_size;
	tcb->func = func;
	tcb->state = task_state;
#ifdef CONFIG_TIME_SLICE
	tcb->time_slice = time_slice;
	tcb->ticks = time_slice;
#endif
	tcb->list = NULL;
	/* port specific initialiation <start>*/
	__init_tcb__(tcb);	
	/* port specific initialiation <end>*/
	tcb->ip =  tcb->func;

	if(task_state == TASK_READY)
		add_to_ready_q(tcb);

	/* add the task to the global list of tasks */
	if(!__task_list_head){
                __task_list_head = tcb;
        }else{
                tmp = __task_list_head;
                while(tmp->list)
                        tmp = tmp->list;

                tmp->list = tcb;
        }

	__curr_num_task ++;

	exit_critical(imask);
	
	return OS_OK;
}

/* Sort based on task priority while adding to the ready list */
int add_to_ready_q(TCB * new)
{
	TCB *start = NULL;	
	TCB *prev = NULL;	
	unsigned int imask;
	int prio = new->prio;

	if(!(new->state == TASK_READY || new->state == TASK_RUNNING) ){
	    pr_error( "add_to_ready_q: task is not in ready state %s, %d\n", new->name, new->state);
	    return OS_ERR;
	}

	new->next = NULL;

	imask = enter_critical();
	if(!task_ready_head){ 
	    task_ready_head = new;
	    exit_critical(imask);
	    return OS_OK;
	}
	
	start = task_ready_head;

	while(start) {	
		
		/* Always head will have the high priority task */
		if(start->prio >= prio) {
			if(start->prio == prio)
			{ /* Same priority task are always added at the after to behave in a co-operative way */
				if(start->next) {
				    if(start->next->prio != prio){ /* if next node has the same prio than move to the next */
				        new->next = start->next;
                                        start->next = new;
                                        goto done;
				    }

				}else{
					new->next = start->next;
					start->next = new;
					goto done;
				}
			}else{
				new->next = start;
				if(start == task_ready_head){
				    /* In case head prio is less */
			            task_ready_head = new;	
				    goto done;	
				}else{
				    /* In case prio is less in the middle */
				    prev->next = new;
				    goto done;
				}
		       }
		}
		prev = start;
		start = start->next;
	}

	/* In case the node should be added at the end */
	prev->next = new; 

done:
     if(__curr_running_task != task_ready_head){
           __need_resched = 1; /* Used for pre-emptive re-scheduling */
     }
        exit_critical(imask);
	return OS_OK;
}

/* remove from ready runqueue  */
int remove_from_ready_q(TCB * rmv)
{
	TCB *start = NULL;	
	TCB *prev = NULL;	
	unsigned int imask;

	imask = enter_critical();

 	start = task_ready_head;

        while(start != NULL) {

                /* Always head will have the high priority task */
                if(start == rmv ) {

                        if(start == task_ready_head){
                                /* In case head is the task */
                                task_ready_head = start->next;
				rmv->next = NULL;
				exit_critical(imask);
				return OS_OK;
                        }else{
                                /* In case task is in the middle */
                                prev->next = start->next;
				rmv->next = NULL;
				exit_critical(imask);
				return OS_OK;
                        }
		}
                prev = start;
                start = start->next;
       }
                /* In case task is at the end */
                prev->next = NULL;
		rmv->next = NULL;
		exit_critical(imask);
		return OS_OK;
}


/* Task cannot abort itself but can be done by other threads 
 * Aborted tasked cannot be resumed */
int abort_task(TCB *tcb)
{
	if(tcb == __curr_running_task)
		return OS_ERR; /* A task cannot abort itself */

	/* releive all the resource that the task curently holds*/
	if(tcb->state == TASK_READY) {
		 remove_from_ready_q(tcb);
	}
	/* TODO Remove form any waitqueues/return resources  and then change the state to task_abort */
	tcb->state = TASK_ABORT;

	return OS_OK;
}

/* Only self complete allowed; Always call this function to gracefully close the task 
 * App has to make sure it returns all the resources before calling task_complete */
int complete_task(TCB *tcb)
{
	if(tcb != __curr_running_task)
		return OS_ERR;

	remove_from_ready_q(tcb);
	tcb->state = TASK_COMPLETE;
	rose_sched();
	return OS_OK;
}

/* Set task priority at runtime */
int set_task_prio(TCB *tcb, int prio)
{
	if((prio == tcb->prio)|| tcb->prio == TASK_COMPLETE || tcb->prio == TASK_ABORT)
		return 0; /* Nothing to do */

	tcb->prio = prio;
	if(tcb->state == TASK_READY) {
		remove_from_ready_q(tcb);
		add_to_ready_q(tcb);
	}
	return OS_OK;
}

/* Task should be either in ABORT or COMPLETE state before calling this function  */
int delete_task(TCB *tcb)
{
	TCB *tmp;

	if((tcb->state == TASK_SUSPEND) || (tcb->state == TASK_READY))
		return OS_ERR;

	/* remove from the global task list */
	if(tcb == __task_list_head){
                        __task_list_head = tcb->list;
        } else {
                        tmp = __task_list_head;
                        while(tmp->list) {
                                if(tmp->list == tcb)
                                        break;
                                tmp = tmp->list;
                        }
                        tmp->list = tmp->list->list;
        }

	return OS_OK;
}

/* Task can be suspended by own or by other task. It has to be resumed by calling task_resume */
int suspend_task(TCB *tcb)
{
	if(!tcb) {

	    remove_from_ready_q(__curr_running_task);
            __curr_running_task->state = TASK_SUSPEND;
            rose_sched();

	}else{
		if(tcb->state == TASK_READY) {
		   remove_from_ready_q(tcb);
                   tcb->state = TASK_SUSPEND;
		}
	}
	return OS_OK;
}

int resume_task(TCB *tcb)
{
	if(tcb->state != TASK_SUSPEND)
		return OS_ERR;

	tcb->state = TASK_READY;
	add_to_ready_q(tcb);

	return OS_OK;
}

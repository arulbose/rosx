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

/* Queues */

#if(CONFIG_QUEUE_COUNT > 0)
/* Max size of block is 16: more than that use pointer */
struct queue * create_queue(char *name, int size_of_block, int num_of_blocks, unsigned int flag)
{
	struct queue *queue;
	if(size_of_block > 16 || size_of_block <= 0 || num_of_blocks == 0){
		pr_error( "create_queue: Check params \n");
		return NULL;
	}
	
	if(NULL == (queue = __alloc_pool(QUEUE_POOL))) {
                pr_error( " create_queue failed\n");
                return NULL;
        }

	if(NULL ==  (queue->start = (char *)salloc(size_of_block * num_of_blocks))){
		pr_error( "create_queue: Insufficent memory for %d bytes", (size_of_block * num_of_blocks));
		 __free_pool(queue, QUEUE_POOL);
		return NULL;
	}
	
	strncpy(queue->name, name, Q_NAME_SIZE);
	queue->size_of_block = size_of_block; 
	queue->num_of_blocks = num_of_blocks; 
	queue->flag |= (QUEUE_EMPTY & flag); /* Initialize Q as empty */
	queue->head = queue->start;
	queue->tail = queue->head;
	queue->task = NULL; /* no task waiting on the queue */
	
	return queue;
}

void delete_queue(struct queue *queue)
{
	if(queue->task) {
		pr_error( "delete_queue: Failed as task is waiting on the queue\n");
		return;
	}
	sfree(queue->start);
	 __free_pool(queue, QUEUE_POOL);
}

#endif

/* TODO Add timeout for the task to wait in the queue; it should not wait indefinitely 
 * Head will always point to the next location to write;i Only one task is allowed to wait on a queue
 */
int read_from_queue(struct queue *p, char *n, int size, int timeout)
{
	unsigned int imask;

        if((p == NULL) || (n == NULL) || (size != p->size_of_block)) {
	    pr_error( "read_from_queue: Check params\n");
            return OS_ERR;
        }

	imask = enter_critical();
	
        if(p->task) {
		exit_critical(imask);
		return E_OS_BUSY;
        }

	/* Check if msg in the queue */
	if((p->head == p->tail) && (!(p->flag & QUEUE_FULL))) {
		/* No msg queue is empty */
		if((p->flag & Q_BLOCK)) {
			/* If queue can be blocked */
		
			if(!timeout) {   		
			    /* If OS_NO_WAIT then suspend the task reading from the queue */
		    	    p->task = __curr_running_task;
		    	    __curr_running_task->state = TASK_SUSPEND;
	 	    	    __remove_from_ready_q(__curr_running_task);
		    	    exit_critical(imask);
		    	    rose_sched();
	 	    	    imask = enter_critical();
			}else{
				struct timer_list timer;
		    	    	p->task = __curr_running_task;
				timeout = ((timeout * 1000)/CONFIG_HZ) + jiffies;
		    	    	exit_critical(imask);
				/* add timer will put the task to sleep */
				__add_timer(&timer, NULL, timeout, __curr_running_task);	
				/* Check if msg available after timeout */
				imask = enter_critical();
				if((p->head == p->tail) && !(p->flag & QUEUE_FULL)){
					exit_critical(imask);
					return E_OS_TIMEOUT;
				}
				imask = enter_critical();
			}
		} else {
				exit_critical(imask);

				return E_OS_UNAVAIL;
		}
	}

	/* Read and copy in to the queue buffer */	
	memcpy(n, p->tail,  p->size_of_block);

	/* roll over if end of queue */
	if((p->tail + p->size_of_block) > (p->start + (p->size_of_block * p->num_of_blocks )))
                p->tail = p->start;
        else
                p->tail += p->size_of_block;

	if(p->head == p->tail) { /* Q is empty Empty: as tails catches head */
		p->flag &= QUEUE_EMPTY;
	}

	 exit_critical(imask);

	return OS_OK;
}

int write_to_queue(struct queue *q, char *n, int size)
{
	unsigned int imask;

        if((q == NULL) || (n == NULL) || (size != q->size_of_block)) {
            pr_error( "read_from_queue: Check params\n");
            return OS_ERR;
        }

        imask = enter_critical();

        /* Check if queue is FULL */
        if(q->head == q->tail) {

                if((q->flag & QUEUE_FULL) && !(q->flag & Q_CYCLIC_FULL)) {
				pr_error( "write_to_queue: Queue FULL!! %s\n", q->name);
				exit_critical(imask);
				return E_OS_UNAVAIL;
			
		}
        }

        /* Read and copy in to the queue buffer */
        memcpy(q->head, n,  q->size_of_block);

        /* roll over if end of queue */
        if((q->head + q->size_of_block) > (q->start + (q->size_of_block * q->num_of_blocks )))
                q->head = q->start;
        else
                q->head += q->size_of_block;

        if(q->head == q->tail) { /* Q is FULL: head catches tail */
                q->flag |= QUEUE_FULL;
        }

	if(q->task) {
	/* Make sure if the task is waiting on this queue and waiting for the timeout on this queue */	
	    if(q->task->timer) {
	       __remove_from_timer_list(q->task->timer, &__active_timer_head);
	       q->task->timer = NULL;
	    }
	  q->task->state = TASK_READY;
	  __add_to_ready_q(q->task);	
	  q->task = NULL; /* Clear the waiting task */	
	}
         
        exit_critical(imask);

	return OS_OK;
}

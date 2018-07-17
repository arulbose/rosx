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

/* Message Queues 
 * Supported functionality :
 * - Static creation of queue
 * - Dynamic creation of queue
 * - Delete of queue
 * - Cyclic queue, Stop when full queue.
 * - Multiple write thread wait with timeout
 * - Single read thread wait with timeout 
 * - Maximum queue block size of 64 bytes; For larger block use 4 bytes to store pointer to message
 *   Planned :
 *   - Get queue info
 *   - Send msg to front of the queue
 *   - prioritize the write threads waiting on the queue
 * */

/** create_queue() - Create a message queue
 * @mq: Message queue control block
 * @name: Name of the queue
 * @size_of_block: Size of a single block; max is 64 bytes
 * @num_of_blocks: Number of size_of_blocks
 * @queue_start: Queue start location
 * @flag: RX_Q_CYCLIC_FULL - When Q is full the Q cycle deleting the earlier messages
 *        RX_Q_STOP_FULL - When Q is full just STOP
 *        RX_Q_BLOCK - If the queue is empty readers wait on the read queue if Q is full writers wait on the write queue
 *        RX_Q_NO_BLOCK - Threads are not allowed to be blocked on the queue.
 */
/* Max size of block is 16 32-bit words: more than that use a pointer */
int rx_create_queue(struct msg_queue *mq, char *name, int size_of_block, int num_of_blocks, void *queue_start, unsigned int flag)
{
	if(size_of_block > 64 || size_of_block <= 0 || num_of_blocks == 0){
		pr_error( "create_queue: Check params \n");
		return OS_ERR;
	}
	
	strncpy(mq->name, name, RX_Q_NAME_SIZE);
	mq->start = queue_start;
	mq->size_of_block = size_of_block; 
	mq->num_of_blocks = num_of_blocks; 
	mq->head = mq->tail = mq->start;
	mq->flag |= (RX_QUEUE_EMPTY & flag); 
	mq->write_task = mq->read_task = NULL;
	
	return OS_OK;
}

/** delete_queue() - Create a message queue
 * @mq: Message queue control block
 */
int rx_delete_queue(struct msg_queue *mq)
{
	RX_TASK *ready;
	unsigned int imask = rx_enter_critical();

        /* Wake up all writers waiting on the queue with error E_OS_Q_DEL */
	while(mq->write_task) {
	    /* Stop any pending timer */
	    if(mq->write_task->timer){
               rx_stop_timer(mq->write_task->timer);
	       mq->write_task->timer = NULL;
            }
	    mq->write_task->state = RX_TASK_READY;
	    mq->write_task->mq = NULL;

	    ready = mq->write_task;
	    mq->write_task = mq->write_task->next;
            __rx_add_to_ready_q(ready);
	}
	
	/* Wake up the readers waiting on the queue */
	if(mq->read_task) {
	    /* Stop any pending timer */
	    if(mq->read_task->timer){
               rx_stop_timer(mq->read_task->timer);
	       mq->read_task->timer = NULL;
            }
	    mq->read_task->state = RX_TASK_READY;
	    mq->read_task->mq = NULL;

            __rx_add_to_ready_q(mq->read_task);
	}

	return OS_OK;

	rx_exit_critical(imask);
}

/** read_from_queue() - Read message from queue
 *  @mq: Message queue control block
 *  @msg_buffer: Buffer to copy the message from the queue
 *  @size : Size of the message to be copied
 *  @Timeout - Wait for a time period in jiffies; If OS_NO_WAIT then return immediately   
 *
 */
int rx_read_from_queue(struct msg_queue *mq, char *msg_buffer, int size, int timeout)
{
	unsigned int imask;
	RX_TASK *ready;

        if((mq == NULL) || (msg_buffer == NULL) || (size != mq->size_of_block)) {
	    pr_error( "read_from_queue: Check params\n");
            return OS_ERR;
        }

	imask = rx_enter_critical();
	
        if(mq->read_task) {
		/* Only one thread is allowed to read from the queue though there is support for multiple writers */
		rx_exit_critical(imask);
		return E_OS_BUSY;
        }

	/* Check if msg in the queue */
	if((mq->head == mq->tail) && (!(mq->flag & RX_QUEUE_FULL))) {
		/* No msg queue is empty */
		if((mq->flag & RX_Q_BLOCK)) {
			/* If task can wait to read */
		
			if(!timeout) {   		
			    /* If OS_WAIT_FOREVER then suspend the task reading from the queue */
		    	    mq->read_task = __rx_curr_running_task;
		    	    __rx_curr_running_task->state = RX_TASK_SUSPEND;
			    __rx_curr_running_task->err = OS_OK;
	 	    	    __rx_remove_from_ready_q(__rx_curr_running_task);
		    	    rx_exit_critical(imask);
		    	    rx_sched();
	 	    	    imask = rx_enter_critical();
			    if(__rx_curr_running_task->err){ /* If queue is deleted */
				    rx_exit_critical(imask);
				    return __rx_curr_running_task->err;
			    }
			}else{
				struct timer_list timer;
		    	    	mq->read_task = __rx_curr_running_task;
				/* Set the timeout to the future */
				timeout = timeout + jiffies;
		    	    	rx_exit_critical(imask);
				/* add timer will put the task to sleep */
				__rx_add_timer(&timer, NULL, timeout, __rx_curr_running_task);	
				/* Check if msg available after timeout */
				imask = rx_enter_critical();
				if((mq->head == mq->tail) && !(mq->flag & RX_QUEUE_FULL)){
					/* Queue is empty hence return error */
					rx_exit_critical(imask);
					return E_OS_TIMEOUT;
				}
			}
		} else {
				rx_exit_critical(imask);

				return E_OS_UNAVAIL;
		}
	}

	/* Read and copy in to the queue buffer */	
	memcpy(msg_buffer, mq->tail,  mq->size_of_block);

	/* roll over if end of queue */
	if((mq->tail + mq->size_of_block) > (mq->start + (mq->size_of_block * mq->num_of_blocks )))
                mq->tail = mq->start;
        else
                mq->tail += mq->size_of_block;

	if(mq->head == mq->tail) { /* Q is empty Empty: as tails catches head */
		mq->flag &= RX_QUEUE_EMPTY;
	}

	/* Wake up any write task waiting on the queue */
	if(mq->write_task) {
	    if(mq->write_task->timer) {
	       __rx_remove_from_timer_list(mq->write_task->timer, &__rx_active_timer_head);
	       mq->write_task->timer = NULL;
	    }

	   mq->write_task->state = RX_TASK_READY;
	   mq->write_task->mq = NULL;
	   ready = mq->write_task;
	   mq->write_task = mq->write_task->next; 
	   __rx_add_to_ready_q(ready);
	}

	 rx_exit_critical(imask);

	return OS_OK;
}

/** write_to_queue() - Write message to queue
 *  @mq: Message queue control block
 *  @msg_buffer: Buffer to copy the message to the queue
 *  @size : Size of the message to be copied
 *  @timeout - Wait for a time period in jiffies; If OS_NO_WAIT then return immediately   
 */
int rx_write_to_queue(struct msg_queue *mq, const char *msg_buffer, int size, int timeout)
{
	unsigned int imask;
	RX_TASK *ride;

        if((mq == NULL) || (msg_buffer == NULL) || (size != mq->size_of_block)) {
            pr_error( "read_from_queue: Check params\n");
            return OS_ERR;
        }

        imask = rx_enter_critical();

        /* Check if queue is FULL */
        if(mq->head == mq->tail) {
                /* If Q is FULL and Q_BLOCK is set then allow thread to block for write */
                if((mq->flag & RX_QUEUE_FULL) && !(mq->flag & RX_Q_CYCLIC_FULL)) {
		    if((mq->flag & RX_Q_BLOCK)) {

                        __rx_curr_running_task->state = RX_TASK_SUSPEND;
			__rx_curr_running_task->err = OS_OK;
			__rx_remove_from_ready_q(__rx_curr_running_task);
		        if(!mq->write_task) {
			    mq->write_task = __rx_curr_running_task;
			}else{
                            ride = mq->write_task;
                            while(ride->next)
				ride = ride->next;
			    ride->next = __rx_curr_running_task;
			}

		        if(!timeout){
			/* OS_WAIT_FOREVER */
                            rx_exit_critical(imask);
			    rx_sched();
			    imask = rx_enter_critical();
			    if(__rx_curr_running_task->err){
			        /* If message queue is deleted then notify the task */
		                rx_exit_critical(imask);
				return __rx_curr_running_task->err;
			    } 

			}else{
				struct timer_list timer;
				/* Set the timeout to the future */
				timeout = timeout + jiffies;
		    	    	rx_exit_critical(imask);
				/* add timer will put the task to sleep */
				__rx_add_timer(&timer, NULL, timeout, __rx_curr_running_task);	
				/* Check if msg available after timeout */
				imask = rx_enter_critical();
				if((mq->head == mq->tail) && (mq->flag & RX_QUEUE_FULL)){
					/* Queue is full after timeout hence return error */
					rx_exit_critical(imask);
					return E_OS_TIMEOUT;
				}
			}

	            }else{
				pr_error( "write_to_queue: Queue FULL!! %s\n", mq->name);
				rx_exit_critical(imask);
				return E_OS_UNAVAIL;
		    }
			
		}
        }

        /* Read and copy in to the queue buffer */
        memcpy(mq->head, msg_buffer,  mq->size_of_block);

        /* roll over if end of queue */
        if((mq->head + mq->size_of_block) > (mq->start + (mq->size_of_block * mq->num_of_blocks )))
                mq->head = mq->start;
        else
                mq->head += mq->size_of_block;

        if(mq->head == mq->tail) { /* Q is FULL: head catches tail */
                mq->flag |= RX_QUEUE_FULL;
        }

	/* Wake up the task waiting to read from this queue */
	if(mq->read_task) {
	    if(mq->read_task->timer) {
	       __rx_remove_from_timer_list(mq->read_task->timer, &__rx_active_timer_head);
	       mq->read_task->timer = NULL;
	    }
	  mq->read_task->state = RX_TASK_READY;
	  mq->read_task->mq = NULL;
	  __rx_add_to_ready_q(mq->read_task);	
	  mq->read_task = NULL; /* Clear the waiting task */
	}
         
        rx_exit_critical(imask);

	return OS_OK;

}

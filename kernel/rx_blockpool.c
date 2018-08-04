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

/* Blocks are always allocated from the head of free_blocks and moved to the head of used_blocks 
 * Wheb the used block is freed they are moved to the head of free_blocks
 * Allocation is of constant time no matter the number of blocks
 * */

int rx_create_block_pool(struct block_pool *pool_data, char *name, unsigned int block_size, void *start_of_pool, unsigned int pool_size)
{
    struct block_chain *temp;
    int i;
	
    if(strlen(name) > (BLOCK_POOL_NAME_LEN - 1)){
        /* Truncate name if exceeds the limit */
        name[BLOCK_POOL_NAME_LEN - 1] = '\0';
    }

	/* Sanity check */
    if(block_size == 0 || pool_size == 0 || (pool_size < (block_size + sizeof(struct block_chain)))){
        pr_error("rx_create_block_pool: Invalid params\n");
        return OS_ERR; 
    }
   
    strcpy((char *)pool_data->name, name); 
    pool_data->start_of_pool = start_of_pool;
    pool_data->pool_size = pool_size;
    pool_data->block_size = block_size;
    pool_data->available = (pool_size/(block_size + sizeof(struct block_chain)));
    pool_data->free_blocks = NULL;
	
	
    for(i = 0; i < pool_data->available; i ++)
    {
        temp = ((struct block_chain *)start_of_pool + (i * (block_size + sizeof(struct block_chain))));
        temp->block = (char *)temp + sizeof(struct block_chain);
        if(pool_data->free_blocks == NULL) {
	    pool_data->free_blocks = temp;
	    temp->next = NULL;
        }else{
			
            /* Add the blocks at the head of free blocks */
            temp->next = pool_data->free_blocks;
            pool_data->free_blocks = temp;
	}
    }
		
    /* Pool is ready ; stamp with the magic number */
    pool_data->magic = 0x55555555;
    return OS_OK;
}

int rx_delete_block_pool(struct block_pool *pool_data)
{
	unsigned int imask = rx_enter_critical();
	if(pool_data->magic == RX_BLOCK_POOL_MAGIC) {
		pool_data->magic = 0x0;
	}else{
            rx_exit_critical(imask);
            return OS_ERR;
	}
    rx_exit_critical(imask);
    return OS_OK;
}

int rx_allocate_block(struct block_pool *pool_data, void **block_ptr, int flags)
{
	struct block_chain *temp;
	unsigned int imask = rx_enter_critical();

	RX_TASK *t;
	if(pool_data->magic != RX_BLOCK_POOL_MAGIC){
            rx_exit_critical(imask);
            return OS_ERR;
	}
	
	if(pool_data->free_blocks == NULL) {
	    if(flags == OS_NO_WAIT) {
	        rx_exit_critical(imask);
		return OS_ERR;
	    }else{
		/* Put the task to sleep */
                __rx_curr_running_task->state = RX_TASK_SUSPEND;
                __rx_curr_running_task->bpool = pool_data;
		__rx_remove_from_ready_q(__rx_curr_running_task);
		__rx_curr_running_task->next = NULL;
		if(!(pool_data->task)) {
		    pool_data->task = __rx_curr_running_task;
		}else{
                    t = pool_data->task;
		    while(t->next)
                        t = t->next;
		    t->next = __rx_curr_running_task;
		}
	    }
		
	}else{
	 /* Assign a block */
	    temp = pool_data->free_blocks;
            pool_data->free_blocks = pool_data->free_blocks->next;
	    /* Pool data pointer is stored for releasing the block */
            temp->next = (struct block_chain *)pool_data;
            *block_ptr = temp->block;	   
	    pool_data->available--; 
	}

        rx_exit_critical(imask);
	return OS_OK;
}

int rx_release_block(void *block_ptr)
{
	RX_TASK *t;
        struct block_chain *start_of_block = (struct block_chain *)((char *)block_ptr - sizeof(struct block_chain));
	struct block_pool *pool_data = (struct block_pool *)start_of_block->next; 
	unsigned int imask = rx_enter_critical();

	if(pool_data->magic != 0x55555555){
	    pr_error("Invalid block pool\n");
		return OS_ERR;
	}

        /* add it back to free block list */	
        if(pool_data->free_blocks == NULL) {
	    pool_data->free_blocks = start_of_block;
	    start_of_block->next = NULL;
        }else{
			
            /* Add the blocks at the head of free blocks */
            start_of_block->next = pool_data->free_blocks;
	    pool_data->free_blocks = start_of_block;
	 }
	pool_data->available++;
        /* Wake up task waiting on the block sleep queue */
	if(pool_data->task != NULL) {
            t = pool_data->task;
	    pool_data->task = pool_data->task->next;
	    t->state = RX_TASK_READY;
            t->bpool = NULL;
	    __rx_add_to_ready_q(t);
	}

        rx_exit_critical(imask);

	return OS_OK;
}

int rx_prioritize_block_pool(struct block_pool *pool_data)
{
    return OS_OK;
}





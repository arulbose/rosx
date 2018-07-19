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

#ifndef __RX_BLOCKPOOL_H__
#define __RX_BLOCKPOOL_H__

#define BLOCK_POOL_NAME_LEN 12
#define RX_BLOCK_POOL_MAGIC 0x55555555

struct block_chain {
	void *block;
	struct block_chain *next;
};

struct block_pool {

    unsigned int magic;
    char *name[BLOCK_POOL_NAME_LEN];
    void *start_of_pool;
    unsigned int pool_size;
    unsigned int block_size;
    unsigned int available;
    struct block_chain *free_blocks;
    RX_TASK *task;
};

/* Block pool APISs */
int rx_create_block_pool(struct block_pool *pool_data, char *name, unsigned int block_size, void *start_of_pool, unsigned int pool_size);
int rx_delete_block_pool(struct block_pool *pool_data);
int rx_allocate_block(struct block_pool *pool_data, void **block_ptr, int flags);
int rx_release_block(void *block_ptr);
int rx_prioritize_block_pool(struct block_pool *pool_data);

#endif /* __RX_BLOCKPOOL_H__ */

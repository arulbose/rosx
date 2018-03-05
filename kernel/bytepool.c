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

static void * __allocate_block(int pos);
static int __round_off(int size);

static unsigned int __total_mempool_used;

struct pool_table {
	int num_of_bytes;
	int num_of_blocks;
	unsigned char *pool_start_ptr;
	int pool_size;
	unsigned int bit_ctrl; /* Each bit corresponds to one block 1: available 0: used */
	int used_blocks;
	int max_used_blocks; /* for memory profiling */
};

const int DeBruijnBitPosition[32] = {
                  				0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
                         			31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        			 };
#define MAX_BLOCK_SIZE 32
#define MAX_BLOCK_FROM_POOL 4096 
/* The below controls the mempool; Increase the pool as required; must add in power of 2 */
static struct pool_table pool_mem[] =  {  
					{1, 		0,0,0,0,0,0},
					{2, 		0,0,0,0,0,0},
					{4, 		CONFIG_4_BYTES_BLOCK,0,0,0,0,0},
					{8, 		CONFIG_8_BYTES_BLOCK,0,0,0,0,0},
					{16, 		CONFIG_16_BYTES_BLOCK,0,0,0,0,0},
					{32, 		CONFIG_32_BYTES_BLOCK,0,0,0,0,0},
					{64, 		CONFIG_64_BYTES_BLOCK,0,0,0,0,0},
					{128, 		CONFIG_128_BYTES_BLOCK,0,0,0,0,0},
					{256, 		CONFIG_256_BYTES_BLOCK,0,0,0,0,0},
					{512, 		CONFIG_512_BYTES_BLOCK,0,0,0,0,0},
					{1024,		CONFIG_1024_BYTES_BLOCK,0,0,0,0,0},
					{2048, 		CONFIG_2048_BYTES_BLOCK,0,0,0,0,0},
					{4096, 		CONFIG_4096_BYTES_BLOCK,0,0,0,0,0}
			     };

/* Will be called from the init() thread */
void bytepool_init()
{
	int i;
	unsigned char *p = __bytepool_start;

	/* Check p is 32 bit aligned */
	if((unsigned int)p & 0x3) {
		pr_panic( "__bytepool_start is not 32 bit aligned %p\n", p);		
	}

	/* check if there is sufficent memory for the pool as per pool_table */
        for(i = 0; i < sizeof(pool_mem)/sizeof(struct pool_table);i ++ )
        {
		if(pool_mem[i].num_of_blocks == 0)
				continue; /* Skip the pool */

		/* Check for max number of blocks for a pool */
		if(pool_mem[i].num_of_blocks > MAX_BLOCK_SIZE)
			pr_panic( "bytepool_init: Invalid block size %d\n", pool_mem[i].num_of_blocks);
                
		p = p + 4; /* Space for guard band */
                p = p + (pool_mem[i].num_of_bytes * pool_mem[i].num_of_blocks);
                p = p + 4; /* Space for guard band */
		if(((unsigned int)(p - __bytepool_start) > CONFIG_BYTEPOOL_SIZE)) {
			pr_panic( "bytepool_init: FAILED: Insufficient memory req %d actual %d\n", (p - __bytepool_start), CONFIG_BYTEPOOL_SIZE);
		}
        }
	__total_mempool_used = (unsigned int)(p - __bytepool_start);

	p = __bytepool_start;
	/* initialize the pool table and the pool  */
	for(i = 0; i < (sizeof(pool_mem)/sizeof(struct pool_table));i ++ )
	{
		 if(pool_mem[i].num_of_blocks == 0){
                                pool_mem[i].bit_ctrl = 0; /* Mark s pool not availble */
                                continue; /* Skip the pool */
                 }

		pool_mem[i].pool_start_ptr = p;
		*(unsigned int *)p = 0xDEADBEEF; /* pool corruption check -- Start marker */
		p = p + 4;

		p = p + (pool_mem[i].num_of_bytes * pool_mem[i].num_of_blocks); 
		*(unsigned int *)p = 0xDEADBEEF; /* pool corruption check marker -- End marker */
		p = p + 4;
		pool_mem[i].bit_ctrl = ~0; /* mark the pool as avilable */
		pool_mem[i].pool_size = (int)p - (int)(pool_mem[i].pool_start_ptr);
	}

}

/* Find the pool->block->bit position to free it using the pointer to be freed */
void sfree(void *ptr)
{
    int i, pos;
/* how to map the pointer with the block */

    for(i = 0; i < (sizeof(pool_mem)/ sizeof(struct pool_table)); i++)
    {
        if(pool_mem[i].num_of_blocks == 0)
                                continue; /* Skip the pool */

        if((unsigned char *)ptr < (pool_mem[i].pool_start_ptr +  pool_mem[i].pool_size)){
                /* Pool found; find the block position to free it  */
                pos = ((unsigned int)ptr - (((unsigned int)pool_mem[i].pool_start_ptr + 4)/pool_mem[i].num_of_bytes));
                pool_mem[i].bit_ctrl |= 1 << pos;
		break;
        }
   }

}
/* Accepts pool type as param and returns the pointer of the block allocated in the pool */
void * __allocate_block(int pos)
{
        unsigned int bit_ctrl  =  pool_mem[pos].bit_ctrl;
        int free_blk = -1;
        if(bit_ctrl){ /* Check if any free slots */
                free_blk = DeBruijnBitPosition[(((unsigned int)((bit_ctrl & -bit_ctrl) * 0x077CB531U)) >> 27)];
                pool_mem[pos].bit_ctrl &= ~(1 << free_blk); /* mark the block bit used*/
                pool_mem[pos].used_blocks ++;
                if(pool_mem[pos].used_blocks > pool_mem[pos].max_used_blocks)
                                pool_mem[pos].max_used_blocks = pool_mem[pos].used_blocks; /* For memory profiling */

                /* return the pointer to the free block */
                return (pool_mem[pos].pool_start_ptr + (4 + (free_blk * pool_mem[pos].num_of_bytes)));
        }

        return NULL; /* No available memoy block */
}

/* Returns a void pointer and accepts the size in bytes */
void *salloc(int size)
{
        int pos = -1;

        /* Check for the max number of bytes that can be allocated */
        if(size > MAX_BLOCK_FROM_POOL || size == 0)
                        return NULL;
        /* Round of the size to the nearest block size */
        size --;
        size = __round_off(size);
        size ++;
	/* find the pool using log2(x)*/
        pos = DeBruijnBitPosition[((unsigned int)(size * 0x077CB531U)) >> 27]; 
        return __allocate_block(pos);
}

int __round_off(int size)
{
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;

        return size;
}


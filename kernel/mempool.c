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

/* struct mutex pool */
#if(CONFIG_MUTEX_COUNT > 0)
static struct mutex __mutex_pool[CONFIG_MUTEX_COUNT - 1];
static unsigned int __mutex_bits = ~0;
#endif
#if(CONFIG_SEMAPHORE_COUNT > 0)
/* struct semaphore pool */
static struct semaphore __semaphore_pool[CONFIG_SEMAPHORE_COUNT - 1];
static unsigned int __semaphore_bits = ~0;
#endif
#if(CONFIG_TIMER_COUNT > 0)
/* struct timer pool */
static struct timer_list __timer_pool[CONFIG_TIMER_COUNT - 1];
static unsigned int __timer_bits = ~0;
#endif
#if(CONFIG_QUEUE_COUNT > 0)
/* struct queue pool */
static struct queue __queue_pool[CONFIG_QUEUE_COUNT - 1];
static unsigned int __queue_bits = ~0;
#endif

#if(CONFIG_EVENT_COUNT > 0)
/* struct event queue pool */
static struct event_group __event_group_pool[CONFIG_EVENT_COUNT - 1];
static unsigned int __event_group_bits = ~0;
#endif

#if(CONFIG_SHIRQ_COUNT > 0)
/* struct event queue pool */
static struct irq __shirq_pool[CONFIG_SHIRQ_COUNT - 1];
static unsigned int __shirq_bits = ~0;
#endif

extern const int DeBruijnBitPosition[32];

#if((CONFIG_MUTEX_COUNT > 0) || (CONFIG_SEMAPHORE_COUNT > 0) || (CONFIG_TIMER_COUNT > 0) || (CONFIG_POOL_COUNT > 0) || (CONFIG_EVENT_COUNT > 0) || (CONFIG_SHIRQ_COUNT))
void __free_pool(void *ptr, int type)
{
	int pos;
	unsigned int imask;

	imask = enter_critical();

	switch(type) {
	#if(CONFIG_MUTEX_COUNT > 0)	
				case MUTEX_POOL:
				{
						if(( (struct mutex *)ptr < &__mutex_pool[0]) || ((struct mutex *)ptr > &__mutex_pool[CONFIG_MUTEX_COUNT - 1])){
							pr_error( "__free_pool invalid pool pointer %p\n", ptr);
						} else {
							pos = (struct mutex *)ptr - &__mutex_pool[0];
							__mutex_bits |= 1 << pos;			
						}
				}
						break;
	#endif
	#if(CONFIG_SEMAPHORE_COUNT > 0)
				case SEMAPHORE_POOL:
				{
                                		if(( (struct semaphore *)ptr < &__semaphore_pool[0]) || ((struct semaphore *)ptr > &__semaphore_pool[CONFIG_SEMAPHORE_COUNT - 1])){
                                        		pr_error( "__free_pool invalid pool pointer %p\n", ptr);
                                		} else {
                                        		pos = (struct semaphore *)ptr - &__semaphore_pool[0];
                                        		__semaphore_bits |= 1 << pos;
                                		}
				}
                                        break;
	#endif
	#if(CONFIG_TIMER_COUNT > 0)
				case TIMER_POOL:
				{
                                		if(( (struct timer_list *)ptr < &__timer_pool[0]) || ((struct timer_list *)ptr > &__timer_pool[CONFIG_TIMER_COUNT - 1])){
                                        		pr_error( "__free_pool invalid pool pointer %p\n", ptr);
                                		} else {
                                        		pos = (struct timer_list *)ptr - &__timer_pool[0];
                                        		__timer_bits |= 1 << pos;
                                		}
				}
                                        break;
	#endif
	#if(CONFIG_POOL_COUNT > 0)
				case QUEUE_POOL:
				{
                                		if(( (struct queue *)ptr < &__queue_pool[0]) || ((struct queue *)ptr > &__queue_pool[CONFIG_QUEUE_COUNT - 1])){
                                        		pr_error( "__free_pool invalid pool pointer %p\n", ptr);
                                		} else {
                                        		pos = (struct queue *)ptr - &__queue_pool[0];
                                        		__queue_bits |= 1 << pos;
                                		}		
                                        break;
				}
	#endif
        #if(CONFIG_EVENT_COUNT > 0)
 				case EVENT_POOL:
				{
                                		if(( (struct event_group *)ptr < &__event_group_pool[0]) || ((struct event_group *)ptr > &__event_group_pool[CONFIG_EVENT_COUNT - 1])){
                                        		pr_error( "__free_pool invalid pool pointer %p\n", ptr);
                                		} else {
                                        		pos = (struct event_group *)ptr - &__event_group_pool[0];
                                        		__event_group_bits |= 1 << pos;
                                		}
                                        break;
				}
        #endif
 	#if(CONFIG_SHIRQ_COUNT > 0)
                                case SHIRQ_POOL:
                                {
                                                if(( (struct irq *)ptr < &__shirq_pool[0]) || ((struct irq *)ptr > &__shirq_pool[CONFIG_SHIRQ_COUNT - 1])){
                                                        pr_error( "__free_pool invalid pool pointer %p\n", ptr);
                                                } else {
                                                        pos = (struct irq *)ptr - &__shirq_pool[0];
                                                        __shirq_bits |= 1 << pos;
                                                }
                                        break;
                                }
        #endif

	}
		exit_critical(imask);
		return;
}
#endif

#if((CONFIG_MUTEX_COUNT > 0) || (CONFIG_SEMAPHORE_COUNT > 0) || (CONFIG_TIMER_COUNT > 0) || (CONFIG_POOL_COUNT > 0) || (CONFIG_EVENT_COUNT > 0) || (CONFIG_SHIRQ_COUNT > 0))
void * __alloc_pool(int type)
{
        void *ptr = NULL;
	int pos = -1;
	unsigned int imask;

	imask = enter_critical();
	
        switch(type) {

#if(CONFIG_MUTEX_COUNT > 0)
                case MUTEX_POOL:
					if(__mutex_bits) {
						pos = DeBruijnBitPosition[(((unsigned int)((__mutex_bits & -__mutex_bits) * 0x077CB531U)) >> 27)];
						__mutex_bits &= ~(1 << pos);
						ptr = (void *)&__mutex_pool[pos];	
					}
                                        break;
#endif
                case SEMAPHORE_POOL:
#if(CONFIG_SEMAPHORE_COUNT > 0)
					 if(__semaphore_bits) {
                                                pos = DeBruijnBitPosition[(((unsigned int)((__semaphore_bits & -__semaphore_bits) * 0x077CB531U)) >> 27)];
                                                __semaphore_bits &= ~(1 << pos);
                                                ptr = (void *)&__semaphore_pool[pos];
                                        }
#endif
                                        break;
                case TIMER_POOL:
#if(CONFIG_TIMER_COUNT > 0)
					 if(__timer_bits) {
                                                pos = DeBruijnBitPosition[(((unsigned int)((__timer_bits & -__timer_bits) * 0x077CB531U)) >> 27)];
                                                __timer_bits &= ~(1 << pos);
                                                ptr = (void *)&__timer_pool[pos];
                		        }
					break;
#endif
                case QUEUE_POOL:
#if(CONFIG_QUEUE_COUNT > 0)
					 if(__queue_bits) {
                                                pos = DeBruijnBitPosition[(((unsigned int)((__queue_bits & -__queue_bits) * 0x077CB531U)) >> 27)];
                                                __queue_bits &= ~(1 << pos);
                                                ptr = (void *)&__queue_pool[pos];
                                        }
#endif
                                        break;

                case EVENT_POOL:
#if(CONFIG_EVENT_COUNT > 0)
                                         if(__event_group_bits) {
                                                pos = DeBruijnBitPosition[(((unsigned int)((__event_group_bits & -__event_group_bits) * 0x077CB531U)) >> 27)];
                                                __event_group_bits &= ~(1 << pos);
                                                ptr = (void *)&__event_group_pool[pos];
                                        }
#endif
                                        break;

                case SHIRQ_POOL:
#if(CONFIG_SHIRQ_COUNT > 0)
                                         if(__shirq_bits) {
                                                pos = DeBruijnBitPosition[(((unsigned int)((__shirq_bits & -__shirq_bits) * 0x077CB531U)) >> 27)];
                                                __shirq_bits &= ~(1 << pos);
                                                ptr = (void *)&__shirq_pool[pos];
                                        }
#endif
                                        break;

        }
		exit_critical(imask);
		return ptr;

}
#endif


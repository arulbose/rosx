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

#ifndef __ROSE_DEFINES_H__
#define __ROSE_DEFINES_H__

#include <asm/interrupt.h>
/* System stack config */
#define CONFIG_STACK_ALLOC_DYNAMIC
#define CONFIG_SYSTEM_STACK_SIZE (8192 * 10)

/* Print buffer */
#define CONFIG_PRINT_BUFFER 
#ifdef CONFIG_PRINT_BUFFER
#define CONFIG_PRINT_BUFFER_SIZE 1024
#endif /* CONFIG_PRINT_BUFFER */

/* Kernel config */
#define CONFIG_HZ 100
#define CONFIG_KERNEL_PREEMPT
#define CONFIG_PRIO_INHERITANCE

/******************************Kernel services config <start> */
/* Semaphores */
#define CONFIG_SEMAPHORE
#define CONFIG_SEMAPHORE_COUNT  10 /* Max count is 32 */ 
/* Mutex */
#define CONFIG_MUTEX
#define CONFIG_MUTEX_COUNT	10 /* Max count is 32 */ 
/* Queues */
#define CONFIG_QUEUE
#define CONFIG_QUEUE_COUNT	10 /* Max count is 32 */ 
/* Events */
#define CONFIG_EVENTS
#define CONFIG_EVENT_COUNT	10 /* Max count is 32 */
/* Timers */
#define CONFIG_TIMERS
#define CONFIG_TIMER_COUNT	10 /* Max count is 32 */
#define CONFIG_TIMER_DEFAULT_STACK_SIZE 512
/* Task */
#define CONFIG_MAX_TASK 32 /* Limited only by the resource available */
#define CONFIG_TASK_STR_LEN 8 /* Max name size of task */
#define CONFIG_TIME_SLICE
/* IRQ*/
#define CONFIG_IRQS		__CONFIG_IRQS
#define CONFIG_IRQ_COUNT	__CONFIG_IRQ_COUNT 
#define CONFIG_SHIRQ_COUNT  10 /* Max count is 32 */

/* Byte pool */
#define CONFIG_BYTEPOOL
#define CONFIG_BYTEPOOL_SIZE 2048
#define CONFIG_4_BYTES_BLOCK	31 /* Max count is 32 */
#define CONFIG_8_BYTES_BLOCK	10 /* Max count is 32 */
#define CONFIG_16_BYTES_BLOCK	10 /* Max count is 32 */
#define CONFIG_32_BYTES_BLOCK	5  /* Max count is 32 */
#define CONFIG_64_BYTES_BLOCK	5  /* Max count is 32 */
#define CONFIG_128_BYTES_BLOCK	0  /* Max count is 32 */
#define CONFIG_256_BYTES_BLOCK	0  /* Max count is 32 */
#define CONFIG_512_BYTES_BLOCK	0  /* Max count is 32 */
#define CONFIG_1024_BYTES_BLOCK	0  /* Max count is 32 */
#define CONFIG_2048_BYTES_BLOCK	0  /* Max count is 32 */
#define CONFIG_4096_BYTES_BLOCK	0  /* Max count is 32 */

/* Device */
#define CONFIG_MAX_DEVICE_DESC 5
#define CONFIG_SERIAL "simx86"
/******************************Kernel services config <end> */
/* log control */
#define PRINT_INFO
#define PRINT_DEBUG
#define PRINT_ERROR

#endif /* __ROSE_DEFINES_H__ */

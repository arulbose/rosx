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

#ifndef __ROSE_RTOS_H__
#define __ROSE_RTOS_H__

/* port file which includes all the defines for the kernel hooks */
#include <asm/osport.h>
#include <asm/switch.h>
#include <rose/string.h>
#include <rose/types.h>
#include <rose/errno.h>

/* rose rtos internal */
#include <rose_version.h>
#include <rose_defines.h>
#include <rose_logger.h>
#include <rose_task.h>
#include <rose_mutex.h>
#include <rose_init.h>
#include <rose_bytepool.h>
#include <rose_queue.h>
#include <rose_timer.h>
#include <rose_irq.h>
#include <rose_sched.h>
#include <rose_events.h>
#include <rose_semaphore.h>
#include <rose_terminal.h>
#include <rose_device.h>
#include <rose_wait.h>

#ifdef CONFIG_PRINT_BUFFER
/* Log controls in defines.h */
#ifdef PRINT_INFO
#define pr_info(fmt, args ...)    __printk_to_buffer(fmt, ## args)
#else
#define pr_info(...)    
#endif

#ifdef PRINT_DEBUG
#define pr_dbg(fmt, args ...)     __printk_to_buffer(fmt, ## args)
#else
#define pr_dbg(...)     
#endif
    
#ifdef PRINT_ERROR
#define pr_error(fmt, args ...)   __printk_to_buffer(fmt, ## args)
#else
#define pr_error(...)   
#endif

#define pr_panic(fmt, args ...)   {               \
                        __printk_to_buffer(fmt, ## args); \
                        }

#else /* CONFIG_PRINT_BUFFER */

/* Log controls in defines.h */
#ifdef PRINT_INFO
#define pr_info(...)	__printk(__VA_ARGS__)
#else
#define pr_info(...)	
#endif

#ifdef PRINT_DEBUG
#define pr_dbg(...)	__printk(__VA_ARGS__)
#else
#define pr_dbg(...)	
#endif

#ifdef PRINT_ERROR
#define pr_error(...) 	__printk(__VA_ARGS__)
#else
#define pr_error(...) 	
#endif

#define pr_panic(...) 	{		\
			__printk(__VA_ARGS__); \
			while(1); \
			}
#endif /*CONFIG_PRINT_BUFFER*/

#endif /* __ROSE_RTOS_H__ */

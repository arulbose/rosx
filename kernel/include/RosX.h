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

#ifndef __ROSX_RTOS_H__
#define __ROSX_RTOS_H__

/* port file which includes all the defines for the kernel hooks */
#include <asm/osport.h>
#include <asm/switch.h>
#include <rosx/string.h>
#include <rosx/types.h>
#include <rosx/errno.h>

/* rosx rtos internal */
#include <rx_version.h>
#include <rx_defines.h>
#include <rx_logger.h>
#include <rx_task.h>
#include <rx_mutex.h>
#include <rx_init.h>
#include <rx_queue.h>
#include <rx_timer.h>
#include <rx_irq.h>
#include <rx_sched.h>
#include <rx_events.h>
#include <rx_semaphore.h>
#include <rx_terminal.h>
#include <rx_device.h>
#include <rx_wait.h>

#ifdef CONFIG_PRINT_BUFFER
/* Log controls in defines.h */
#ifdef PRINT_INFO
#define pr_info(fmt, args ...)    __rx_printk_to_buffer(fmt, ## args)
#else
#define pr_info(...)    
#endif

#ifdef PRINT_DEBUG
#define pr_dbg(fmt, args ...)     __rx_printk_to_buffer(fmt, ## args)
#else
#define pr_dbg(...)     
#endif
    
#ifdef PRINT_ERROR
#define pr_error(fmt, args ...)   __rx_printk_to_buffer(fmt, ## args)
#else
#define pr_error(...)   
#endif

#define pr_panic(fmt, args ...)   {               \
                        __rx_printk_to_buffer(fmt, ## args); \
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

#endif /* __ROSX_RTOS_H__ */

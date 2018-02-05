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
#include "osport.h"
#include "switch.h"
#include "string.h"
/* rose rtos internal */
#include "defines.h"
#include "type.h"
#include "task.h"
#include "mutex.h"
#include "init.h"
#include "bytepool.h"
#include "mempool.h"
#include "queue.h"
#include "timer.h"
#include "irq.h"
#include "sched.h"
#include "events.h"
#include "semaphore.h"

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

#endif /* __ROSE_RTOS_H__ */

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

#ifndef __KERNEL_TYPE_H__
#define __KERNEL_TYPE_H__
#include <asm/types.h>

/* Special values for "timeout" parameter. */
#define OS_NO_WAIT                0     /**< The blocking function returns immediately */
#define OS_WAIT_FOREVER           -1    /**< The blocking function will wait indefinitely */

/* system wide error codes */
typedef enum{
	OS_OK = 0,
	OS_ERR = -1,
	E_OS_TIMEOUT = -2,
	E_OS_BUSY = -3,
	E_OS_UNAVAIL = -4,
	E_OS_ASSERT_FAIL = -5,
	E_OS_ERR_NO_MEMORY = -6,
	E_OS_ERR_OVERFLOW = -7,
	E_OS_ERR_UNKNOWN,
}OS_ERR_TYPE;

#define TRUE 1
#define FALSE 0
#define USED 1
#define EMPTY 0

typedef unsigned int            __kernel_size_t;
typedef int                     __kernel_ssize_t;

#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t         size_t;
#endif

#ifndef _SSIZE_T 
#define _SSIZE_T 
typedef __kernel_ssize_t        ssize_t;
#endif

#ifndef NULL
#define NULL 0
#endif

#if 0
#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#endif /* __KERNEL_TYPE_H__ */

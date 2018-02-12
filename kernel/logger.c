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
#include <stdarg.h>

static int print_buffer_ready = 0;

/* Helper macro to align buffer pointer to cycle within the boundary */
#define MOVE_BUFFER_POINTER(ptr, size) { \
			if((ptr + size) > (__printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE) ) \
				ptr = __printk_buffer_start_ptr; \
			else	\
				ptr = ptr + size; \
			}

#define CHECK_AND_FIX_BUFFER_OVERFLOW(ptr, size) { \
							if((__printk_buffer_head + (size + 1)) > (__printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE)){ \
						        	/* Size cannot fit move the head to start of the buffer and also set to skipped mem to NULL */ \
        							memset(__printk_buffer_start_ptr, '\0', (__printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE - __printk_buffer_head)); \
       								 __printk_buffer_head = __printk_buffer_start_ptr; \
    							} \
						}
/* Store the print output in the buffer; Take care of buffer overflow and truncate and complain if the a single print exceeds the maximum capacity of the buffer
*/
void __printk_to_buffer(const char *fmt, ...)
{
    va_list arg;
    int size;

    /* This should be used with uinlikely for compiler optimization */
    if(!print_buffer_ready){
    //    __early_printk(fmt, ...);
    }

    va_start(arg, fmt);
    size = snprintf(NULL, 0, fmt, arg);

    /* Check if the size fits within the boundary; if yes, drop the string and complain */
    if((size + 1) > CONFIG_PRINT_BUFFER_SIZE) {
 	pr_error("Invalid print request; exceeeds print buffer capacity Requested: %d Available: %d\n" , (size + 1), CONFIG_PRINT_BUFFER_SIZE );
        return;
     }

    CHECK_AND_FIX_BUFFER_OVERFLOW(__printk_buffer_head, (size + 1))

    /* All good */
    vsprintf(__printk_buffer_head, fmt, arg);
    __printk_buffer_head = __printk_buffer_head + (size + 1);

    va_end (arg);
}

void rose_logger_thread()
{
    while(1) {

        if(__printk_buffer_head != __printk_buffer_tail) {
           __printk("%c", *__printk_buffer_tail);
           MOVE_BUFFER_POINTER(__printk_buffer_tail, 1)
	}

    }
}

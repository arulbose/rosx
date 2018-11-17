/* Rosx RT-Kernel
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
#include <stdarg.h>

#ifdef CONFIG_PRINT_BUFFER
static int rx_print_buffer_ready = 0;

/* Store the print output in the buffer; Take care of buffer overflow and truncate and complain if the a single print exceeds the maximum capacity of the buffer
*/
void __rx_printk_to_buffer(const char *fmt, ...)
{
    va_list arg;
    int size;
    unsigned int imask;

    va_start(arg, fmt);
    size = vsnprintf(NULL, 0, fmt, arg);

    /* Check if the size fits within the boundary; if yes, drop the string and complain */
    if((size + 1) > CONFIG_PRINT_BUFFER_SIZE) {
 	pr_error("Invalid print request; exceeeds print buffer capacity Requested: %d Available: %d\n" , (size + 1), CONFIG_PRINT_BUFFER_SIZE );
        return;
     }
    imask = rx_enter_critical();

    if((__rx_printk_buffer_head + (size + 1)) > (__rx_printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE)){
        /* Size cannot fit move the head to start of the buffer and also set to skipped mem to NULL */ 
        memset(__rx_printk_buffer_head, '\0', ((__rx_printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE) - __rx_printk_buffer_head));
        __rx_printk_buffer_head = __rx_printk_buffer_start_ptr;
     } 

    /* All good */
    vsprintf(__rx_printk_buffer_head, fmt, arg);
    __rx_printk_buffer_head = __rx_printk_buffer_head + (size + 1);

    rx_exit_critical(imask);

    va_end (arg);
}

void rx_logger_thread()
{
    int dd;
    RX_DEFINE_WAITQUEUE(w);

    rx_print_buffer_ready = 1;
    rx_early_printk("rosx_logger_thread ready\n");

    if (0 < (dd = rx_dev_open(CONFIG_SERIAL, 0))) {
       rx_early_printk("Failed to open serial device\n");
       while(1); 
    }

    while(1) {
        if(0 == rx_wait_queue(&w, (__rx_printk_buffer_head != __rx_printk_buffer_tail))) {
           rx_dev_write(dd, __rx_printk_buffer_tail, 1);
           if((__rx_printk_buffer_tail + 1) > (__rx_printk_buffer_start_ptr + CONFIG_PRINT_BUFFER_SIZE) ) {
              /* Buffer roll over */
               __rx_printk_buffer_tail = __rx_printk_buffer_start_ptr;
           }else{
               __rx_printk_buffer_tail = __rx_printk_buffer_tail + 1;
           }

	}
    }
}

#endif

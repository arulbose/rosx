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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef CONFIG_PRINT_BUFFER

#include <RosX.h>

void __rx_printk_to_buffer(const char *fmt, ...);
void rx_logger_thread(void);

char *__rx_printk_buffer_head;
char *__rx_printk_buffer_tail;
char *__rx_printk_buffer_start_ptr;

#endif

#endif /* __LOGGER_H__ */

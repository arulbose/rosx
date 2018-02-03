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
#include "osport.h"
#include <RoseRTOS.h>

/* <TUNE CODE> */
void exit_critical (unsigned int key)
{
    if(__context || !(__critical))
	return; /* If in IRQ context return doing nothing */

    /* <ADD to exit critical> */
    __critical = 0;
}

/* Interrupts are simulated by using signals in UNIX */
unsigned int enter_critical (void)
{
    if(__context || __critical) /* if already in IRQ or in critical than do nothing */
	return 0;

    /* <ADD to enter  critical> */
    __critical = 1;
      
    return 0;
}


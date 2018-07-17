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
#include <signal.h>

void __restore_irq_global(unsigned int key)
{
	if(__context || !(__critical))
		return; /* If in IRQ context return doing nothing */

	if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0) {
		pr_info("rx_enter_critical: sigprocmask failed");
		exit(1);
	}
	__critical = 0;
}

/* Interrupts are simulated by using signals in UNIX */
unsigned int __disable_irq_global(void)
{
    if(__context || __critical) /* if already in IRQ or in critical than do nothing */
		return 0;

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
		pr_info("rx_enter_critical: sigprocmask failed");
		exit(1);
    }
    __critical = 1;
      
    return 0;
}

void __enable_irq(unsigned int num)
{
 //do nothing
}

void __disable_irq(unsigned int num)
{
//do nothing
}

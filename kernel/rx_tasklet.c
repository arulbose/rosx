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

#include <RosX.h>

/* Under construction */

int rx_create_tasklet(struct tasklet *t, void(*func)(unsigned long), unsigned long data);
void rx_delete_tasklet(struct tasklet *);
void rx_enable_tasklet(struct tasklet *);
void rx_disable_tasklet(struct tasklet *);
void rx_schedule_tasklet(struct tasklet *);

void rx_bh_thread()
{
    while(1) {
        rx_suspend_task(MYSELF);
    }
    
}

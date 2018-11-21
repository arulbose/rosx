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

#ifndef __RX_TASKLET_H__
#define __RX_TASKLET_H__

/* 1. Support static and dynamic allocation.
   2. After initialization the tasklet can be disabled or enabled. 
   3. When scheduled multiple times if the tasklet did not had a chance to run it runs only once.
   4. If tasklet it scheduled while it is running it will run again after it continous. 
   5. tasklet can runs with high priority. 
*/

struct tasklet {
   int status;
   void (*func)(unsigned long); 
   unsigned long data;
   struct tasklet *next;
};

enum tasklet_status{
    __RX_DISABLE_TASKLE,
    __RX_ENABLE_TASKLET,
    __RX_SCHED_TASKLET,
    __RX_RUNNING_TASKLET,
};

#define RX_DEFINE_TASKLET(name, func, data)    \
      struct tasklet (name) = __TASKLET_INIT(name, func, data)

#define __TASKLET_INIT(m)           \
           {                      \
             .func = func,  \
             .data = data,       \
           }

int rx_create_tasklet(struct tasklet *t, void(*func)(unsigned long), unsigned long data);
void rx_delete_tasklet(struct tasklet *);
void rx_enable_tasklet(struct tasklet *);
void rx_disable_tasklet(struct tasklet *);
void rx_schedule_tasklet(struct tasklet *);
void rx_bh_thread(void);

#endif /* __RX_TASKLET_H__ */

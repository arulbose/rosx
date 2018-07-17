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

#ifndef __RX_MUTEX_H__
#define __RX_MUTEX_H__


/******* MUTEX *******************/
struct mutex{

        int lock;
	RX_TASK *owner; /* task which currently owns the mutex lock */
        RX_TASK *task; /* list of task waiting for the mutex lock */
	struct mutex *next;
};

enum{
__RX_MUTEX_LOCKED,
__RX_MUTEX_UNLOCKED
}_RX_MUTEX_LOCK;

#define RX_MUTEX_WAIT_LOCK 1
#define RX_MUTEX_NO_WAIT_LOCK 2

/* -------------- Application system calls ---------------- */

#define RX_DEFINE_MUTEX(m)    \
      struct mutex (m) = __MUTEX_INIT(m)

#define __MUTEX_INIT(m)           \
           {                      \
             .lock = __UNLOCKED,  \
             .owner = NULL,       \
             .task = NULL,        \
             .next = NULL,        \
           }

int rx_create_mutex(struct mutex *m);
void rx_delete_mutex(struct mutex *p);
int rx_mutex_unlock(struct mutex *p);
int rx_mutex_lock(struct mutex *p, int ticks); /* Ticks or Jiffies */

#endif

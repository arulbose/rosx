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

#ifndef __MUTEX_H__
#define __MUTEX_H__


/******* MUTEX *******************/
struct mutex{

        int lock;
	TCB *owner; /* task which currently owns the mutex lock */
        TCB *task; /* list of task waiting for the mutex lock */
	struct mutex *next;
};

enum{
__LOCKED,
__UNLOCKED
}_MUTEX_LOCK;

#define WAIT_LOCK 1
#define NO_WAIT_LOCK 2

/* -------------- Application system calls ---------------- */

#define DEFINE_MUTEX(l)    \
      struct mutex (l) = __MUTEX_INIT(l)

#define __MUTEX_INIT(l)           \
           {                      \
             .lock = __UNLOCKED,  \
             .owner = NULL,       \
             .task = NULL,        \
             .next = NULL,        \
           }

#if(CONFIG_MUTEX_COUNT > 0)
struct mutex * create_mutex(void);
void delete_mutex(struct mutex *p);
#endif
int mutex_unlock(struct mutex *p);
int mutex_lock(struct mutex *p, unsigned int ticks);

#endif

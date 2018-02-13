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

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

struct semaphore {

    int init_val;
    int curr_val;
    TCB *task; /* task waiting on the semaphore */
    struct semaphore *next;
};

/* -------------- Application system calls ---------------- */
void init_semaphore(struct semaphore *sem, int val);
#if(CONFIG_SEMAPHORE_COUNT > 0)
struct semaphore * create_semaphore(int val);
void delete_semaphore(struct semaphore *sem);
#endif
int semaphore_post(struct semaphore *sem);
int semaphore_wait(struct semaphore *sem, int timeout);

#endif

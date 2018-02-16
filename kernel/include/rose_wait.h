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

#ifndef __ROSE_WAIT_H__
#define __ROSE_WAIT_H__

struct wait_queue{
        TCB *task; /* task waiting for the event */
        /* Add all local tracking stuff here */
        struct wait_queue *next; /* Will be hooked to the system wait queue list */
        struct wait_queue *prev; /* Will be hooked to the system wait queue list */
};

#define wait_event(wq, condition)                                       \
({                                                                      \
        if (!(condition))                                               \
                __wait_event(wq, condition);                            \
})

#define __wait_event(struct wait_queue *wq, int condition)              \
do {                                                                    \
        for(;;){                                                        \
           add_to_wait_queue(wq, TASK_INTERRUPTIBLE);                   \
           if(condition)                                                \
              break;                                                    \
           rose_sched();                                                \
        }                                                               \
}while(0)


/*  For indefinite wait  */
#define wait_on(wq)                                                     \
({                                                                      \
         __wait_on(wq);                                                 \
})

#define __wait_on(struct wait_queue *wq)                                \
do {                                                                    \
        for(;;){                                                        \
           add_to_wait_queue(wq, TASK_UNINTERRUPTIBLE);                 \
           rose_sched();                                                \
        }                                                               \
}while(0)

#define DEFINE_WAITQUEUE(wq)    \
      struct waitqueue wq = __WAITQUEUE_INIT(wq)

#define __WAITQUEUE_INIT(wq)    \
           {                    \
             .task = NULL;       \
             .next = NULL;       \
             .prev = NULL;       \
           }

int add_to_wait_queue(struct wait_queue *wq);
int wake_up(struct wait_queue *);
void rose_event_thread();

#endif

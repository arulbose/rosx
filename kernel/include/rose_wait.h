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

/* Wait/Wake macro reference - Linux include/linux/wait.h */

#ifndef __ROSE_WAIT_H__
#define __ROSE_WAIT_H__

#include<rose_task.h>

/* Below are the currently supported
 * wait_queue(struct wait_queue *wq, condition)
 * wait_on(struct wait_queue *wq)
 * wake_up(struct wait_queue *)
 */

struct wait_queue{
        TCB *task; /* task waiting for the event */
        /* Add all local tracking stuff here */
        struct wait_queue *next;
        struct wait_queue *prev;
};

int __finish_wait();
int __add_to_wait_queue(struct wait_queue *wq, int, int);

#define wait_queue_timeout(wq, condition, timeout)                      \
({                                                                      \
        int __ret = 0;                                                  \
        if (!(condition)){                                              \
            __wait_queue_timeout(wq, condition, timeout, __ret);        \
        }                                                               \
        __ret;                                                          \
})

#define __wait_queue_timeout(wq, condition, timeout, ret)               \
do {                                                                    \
        for(;;){                                                        \
           __add_to_wait_queue(wq, TASK_INTERRUPTIBLE, timeout);        \
           rose_sched();                                                \
           if((condition) || (E_OS_TIMEOUT == ((wq)->timeout) )) {      \
               break;                                                   \
            }                                                           \
          (ret) =  __finish_wait(wq);                                   \
        }                                                               \
}while(0)

#define wait_queue(wq, condition)                                       \
({									\
        int __ret = 0;                                                  \
        if (!(condition))                                               \
                __wait_queue(wq, condition, __ret);                     \
        __ret;                                                          \
})

#define __wait_queue(wq, condition, ret)                                \
do {                                                                    \
        for(;;){                                                        \
           __add_to_wait_queue(wq, TASK_INTERRUPTIBLE, 0);              \
           rose_sched();                                                \
           if(condition) {                                              \
               break;                                                   \
            }                                                           \
        }                                                               \
       (ret) =  __finish_wait(wq);                                      \
}while(0)

/*  For indefinite wait  */
#define wait_on(wq)                                                     \
({                                                                      \
        int __ret = 0;                                                  \
         __wait_on(wq, __ret);                                          \
        __ret;                                                          \
})

#define __wait_on(wq, ret)                                              \
do {                                                                    \
        __add_to_wait_queue(wq, TASK_UNINTERRUPTIBLE, 0);               \
        rose_sched();                                                   \
       (ret) =  __finish_wait(wq);                                      \
}while(0)

/*  For indefinite wait  */
#define wait_on_timeout(wq, timeout)                                    \
({                                                                      \
        int __ret = 0;                                                  \
         __wait_on_timeout(wq, timeout, __ret);                         \
        __ret;                                                          \
})

#define __wait_on_timeout(wq, timeout, ret)                             \
do {                                                                    \
       __add_to_wait_queue(wq, TASK_UNINTERRUPTIBLE, timeout);          \
       rose_sched();                                                    \
       (ret) =  __finish_wait(wq);                                      \
}while(0)


#define DEFINE_WAITQUEUE(wq)     \
    struct wait_queue wq = __WAITEVENT_INIT(wq)

#define __WAITEVENT_INIT(wq)     \
           {                     \
             .task = NULL,       \
             .next = NULL,       \
             .prev = NULL,       \
           }

void __rose_wake(void);
int wakeup(struct wait_queue *);

#endif /* __ROSE_WAIT_H__ */

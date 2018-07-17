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

/* Wait/Wake macro reference - Linux include/linux/wait.h */

#ifndef __ROSX_WAIT_H__
#define __ROSX_WAIT_H__

#include<rx_task.h>

/* Below are the currently supported
 * wait_queue(struct wait_queue *wq, condition)
 * wait_on(struct wait_queue *wq)
 * wake_up(struct wait_queue *)
 */

struct wait_queue{
        RX_TASK *task; /* task waiting for the event */
        /* Add all local tracking stuff here */
        struct wait_queue *next;
        struct wait_queue *prev;
};

int __rx_finish_wait();
int __rx_add_to_wait_queue(struct wait_queue *wq, int, int, struct timer_list *);

#define rx_wait_queue_timeout(wq, condition, timeout)                       \
({                                                                       \
        int __ret = 0;                                                   \
        struct timer_list timer;                                         \
        if (!(condition)){                                               \
            __rx_wait_queue_timeout(wq, condition, timeout, timer, __ret);  \
        }                                                                \
        __ret;                                                           \
})

#define __rx_wait_queue_timeout(wq, condition, timeout, timer, ret)         \
do {                                                                     \
        for(;;){                                                         \
           __rx_add_to_wait_queue(wq, RX_TASK_INTERRUPTIBLE, timeout, &timer); \
           rx_sched();                                                 \
           if((condition) || (E_OS_TIMEOUT == ((wq)->timeout) )) {       \
               break;                                                    \
            }                                                            \
          (ret) =  __rx_finish_wait(wq);                                    \
        }                                                                \
}while(0)

#define rx_wait_queue(wq, condition)                                       \
({									\
        int __ret = 0;                                                  \
        if (!(condition))                                               \
                __rx_wait_queue(wq, condition, __ret);                     \
        __ret;                                                          \
})

#define __rx_wait_queue(wq, condition, ret)                                \
do {                                                                    \
        for(;;){                                                        \
           __rx_add_to_wait_queue(wq, RX_TASK_INTERRUPTIBLE, 0, 0);           \
           rx_sched();                                                \
           if(condition) {                                              \
               break;                                                   \
            }                                                           \
        }                                                               \
       (ret) =  __rx_finish_wait(wq);                                      \
}while(0)

/*  For indefinite wait  */
#define rx_wait_on(wq)                                                     \
({                                                                      \
        int __ret = 0;                                                  \
         __rx_wait_on(wq, __ret);                                          \
        __ret;                                                          \
})

#define __rx_wait_on(wq, ret)                                              \
do {                                                                    \
        __rx_add_to_wait_queue(wq, RX_TASK_UNINTERRUPTIBLE, 0, 0);            \
        rx_sched();                                                   \
       (ret) =  __rx_finish_wait(wq);                                      \
}while(0)

/*  For indefinite wait  */
#define rx_wait_on_timeout(wq, timeout)                                    \
({                                                                      \
        int __ret = 0;                                                  \
        struct timer_list timer;                                        \
         __rx_wait_on_timeout(wq, timeout, timer, __ret);                  \
        __ret;                                                          \
})

#define __rx_wait_on_timeout(wq, timeout, timer, ret)                      \
do {                                                                    \
       __rx_add_to_wait_queue(wq, RX_TASK_UNINTERRUPTIBLE, timeout, &timer);  \
       rx_sched();                                                    \
       (ret) =  __rx_finish_wait(wq);                                      \
}while(0)


#define RX_DEFINE_WAITQUEUE(wq)     \
    struct wait_queue wq = __RX_WAITEVENT_INIT(wq)

#define __RX_WAITEVENT_INIT(wq)     \
           {                     \
             .task = NULL,       \
             .next = NULL,       \
             .prev = NULL,       \
           }

void __rx_wake(void);
int rx_wakeup(struct wait_queue *);

#endif /* __ROSX_WAIT_H__ */

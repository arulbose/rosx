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

#ifndef __RX_TASK_H__
#define __RX_TASK_H__
#define RX_TASK_STR_LEN CONFIG_TASK_STR_LEN /* Fixed */
/**************** OS  Task definitions  **********************************/
/* RX_TASK-Task control block, expected to have all the details of the task
 * The number of tasks is limited only by the resource available; Rosx kernel by itself
 * does not limit the number of tasks
 */
struct __rx_task_control_block
{
	void *preempt;           /* DO NOT change the 'preempt' order in the structure> used to Save EIP(current instruction pointer) during preempt */
        char name[RX_TASK_STR_LEN]; /* Task name; Use LEN multiples of 4 bytes to avoid un-aligned access */
        void *stack_start_ptr;   /* Start of stack for the task */
        void *curr_stack_ptr;    /* Current stack pointer for the task */
        void *ip;                /* Current instruction pointer/program counter */
        void *bp;                /* Stack frame pointer */
        int stack_size;          /* Stack size for the task */
        int prio;                /* Current task priority value 0 has the hisghest priority */
        void (*func)(void);      /* Pointer to task handler */
        int state;               /* Task current state */
#ifdef CONFIG_TIME_SLICE
	int time_slice;          /* Time slice the task supposed to run; during task inittialization time_slice = ticks */
	int ticks;               /* Ticks when 0 task will be pre-empted in co-operative mode */
#endif
#ifdef CONFIG_PRIO_INHERITANCE
	int orig_prio;           /* Priority during task creation */
#endif
        unsigned int event_flag; /* Task event flag */
        unsigned int event_recv; /* Task received events */
        int timeout;             /* Used for store if the task is waiting on a waitqueue/semaphore */
	struct timer_list *timer;/* Pointer to task timer  */
	struct semaphore *sem;   /* Pointer to task semaphore */
	struct mutex *mutex;     /* Pointer task mutex */
	struct event_group *event; /* Pointer to task event group */
        struct wait_queue *wq;     /* Pointer to task wait event */
	struct msg_queue *mq;          /* pointer to a message queue */
	struct __rx_task_control_block *next; /* pointer to the next task kcontrol block */
	struct __rx_task_control_block *list; /* used to add in the global list of task */
	int err; /* Used to store the error code */
};

typedef struct __rx_task_control_block RX_TASK;
#define RX_TASK_LEAST_PRIO (CONFIG_MAX_TASK - 1) /* CONFIG_MAX_TASK is defined by apps/define.h */
#define RX_TASK_HIGH_PRIO 0

/* Globals; Application should not to use it directly instead use the revelvant apis */

int __rx_curr_num_task;
#ifdef CONFIG_STACK_ALLOC_DYNAMIC
void * __rx_stack_start_ptr;
void * __rx_curr_stack_ptr;
#endif
RX_TASK *__rx_curr_running_task;
RX_TASK *__rx_task_ready_head;
RX_TASK *__rx_task_list_head;
RX_TASK __rx_init_task;

int __rx_add_to_ready_q(RX_TASK *p);
int __rx_remove_from_ready_q(RX_TASK *tid);

/* Task state */
enum{
        RX_TASK_SUSPEND,
        RX_TASK_READY,
        RX_TASK_RUNNING,
        RX_TASK_COMPLETE,
        RX_TASK_ABORT,
        RX_TASK_INTERRUPTIBLE,
        RX_TASK_UNINTERRUPTIBLE,

}RX_TASK_STATE;

/* Macro used for self suspend */
#define MYSELF 0
/* -------------- Application system calls ---------------- */
int rx_create_task(RX_TASK *, char *task_name, int prio, void *stack_ptr, int stack_size, void (*func)(void), int task_state, int time_slice);
int rx_delete_task(RX_TASK *);
int rx_complete_task(RX_TASK *); 
int rx_set_task_prio(RX_TASK *, int prio);
int rx_suspend_task(RX_TASK *);
int rx_resume_task(RX_TASK *);
int rx_abort_task(RX_TASK *);

#endif

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

#ifndef __TASK_H__
#define __TASK_H__
#define TASK_STR_LEN 8 /* Fixed */
/**************** OS  Task definitions  **********************************/
/* TCB-Task control block, expected to have all the details of the task
 * The number of tasks is limited only by the resource available; Rose kernel by itself
 * does not limit the number of tasks
 */
struct __task_control_block
{
        char name[TASK_STR_LEN]; /* Task name */
        void *stack_start_ptr;   /* Start of stack for the task */
        void *curr_stack_ptr;    /* Current stack pointer for the task */
        void *ip;                /* Current instruction pointer/program counter */
        void *bp;                /* Stack frame pointer */
	void *preempt;           /* Save EIP(current instruction pointer) during preempt */
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
	struct timer_list *timer;/* Pointer to task timer  */
	struct semaphore *sem;   /* Pointer to task semaphore */
	struct mutex *mutex;     /* Pointer task mutex */
	struct event_group *event; /* Pointer to task event group */
        struct wait_q *wq;         /* Pointer to task wait queue */
	struct __task_control_block *next; /* pointer to the next task kcontrol block */
	struct __task_control_block *list; /* used to add in the global list of task */
};

typedef struct __task_control_block TCB;
#define LEAST_PRIO (CONFIG_MAX_TASK - 1) /* CONFIG_MAX_TASK is defined by apps/define.h */
#define HIGH_PRIO 0

/* Globals; Application should not to use it directly instead use the revelvant apis */

int __curr_num_task;
#ifdef CONFIG_STACK_ALLOC_DYNAMIC
void * __stack_start_ptr;
void * __curr_stack_ptr;
#endif
TCB *__curr_running_task;
TCB *task_ready_head;
TCB *__task_list_head;
TCB __init_task;

int add_to_ready_q(TCB *p);
int remove_from_ready_q(TCB *tid);

/* Task state */
enum{
        TASK_SUSPEND,
        TASK_READY,
        TASK_RUNNING,
        TASK_COMPLETE,
        TASK_ABORT,
        TASK_INTERRUPTIBLE,
        TASK_UN_INTERRUPTIBLE,

}TASK_STATE;

/* macro used for self suspend */
#define MYSELF 0
/* -------------- Application system calls ---------------- */
int create_task(TCB *, char *task_name, int prio, void *stack_ptr, int stack_size, void (*func)(void), int task_state, int time_slice);
int delete_task(TCB *);
int complete_task(TCB *); 
int set_task_prio(TCB *, int prio);
int suspend_task(TCB *);
int resume_task(TCB *);
int abort_task(TCB *);

#endif

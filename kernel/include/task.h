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
/* TCB-Task control block, expected to have all the details of the task */
struct __task_control_block
{
        char name[TASK_STR_LEN];
        void *stack_start_ptr;
        void *curr_stack_ptr;
        void *ip;  /* PC*/
        void *bp; /* Stack frame pointer  */
	void *preempt; /* Save EIP during preempt*/
        int stack_size;
        int prio;
        void (*func)(void);
        int state;
#ifdef CONFIG_TIME_SLICE
	int time_slice; /* time slice the task supposed to run */
	int ticks; /* ticks when 0 task will be pre-empted in co-operative mode */
#endif
#ifdef CONFIG_PRIO_INHERITANCE
	int orig_prio;
#endif
        unsigned int event_flag;
        unsigned int event_recv;
	struct timer_list *timer;
	struct semaphore *sem;
	struct mutex *mutex;
	struct event_group *event;
	struct __task_control_block *next;
	struct __task_control_block *list; /* used to add in the global list of task */
};

typedef struct __task_control_block TCB;
#define TASK_PRIO_MAX 32  /* limited only by resource available */
#define LEAST_PRIO (CONFIG_MAX_TASK - 1)

/* Globals */
int __curr_num_task;
#ifdef CONFIG_STACK_ALLOC_DYNAMIC
void * __stack_start_ptr;
void * __curr_stack_ptr;
#endif

TCB *__curr_running_task;

/* OS */
TCB __init_task;
TCB *task_ready_head;
TCB *__task_list_head;

unsigned int __temp;

/* Not inteded for the application to use it directly */
int add_to_ready_q(TCB *p);
int remove_from_ready_q(TCB *tid);

enum{
        TASK_SUSPEND,
        TASK_READY,
        TASK_RUNNING,
        TASK_COMPLETE,
        TASK_ABORT,

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

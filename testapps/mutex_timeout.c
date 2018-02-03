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

#include <RoseRTOS.h>

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static TCB idle_tcb;
static TCB task_1_tcb;
static TCB task_2_tcb;
static TCB task_3_tcb;

struct mutex MY_MUTEX;

struct priv {
       int num;
};

struct priv MY_PRIV = {10};

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    create_task(&idle_tcb,"idle", LEAST_PRIO, 0, 8192, idle_task, TASK_READY, 0);	
    create_task(&task_1_tcb,"task1", 3, 0, 8192, task_1, TASK_READY, 0);	
    create_task(&task_2_tcb,"task2", 2, 0, 8192, task_2, TASK_READY, 0);	
    create_task(&task_3_tcb,"task3", 1, 0, 8192, task_3, TASK_READY, 0);
    
    init_mutex(&MY_MUTEX);	
    rose_sched();
}

void task_3(void)
{
	while(1) {
			
		mutex_lock(&MY_MUTEX, OS_WAIT_FOREVER);
		ssleep(12);
		mutex_unlock(&MY_MUTEX);
		suspend_task(MYSELF);
	}

}

void task_2(void)
{
	pr_info("entering task_2\n");

	while(1) {
#if 1
		if(OS_OK != mutex_lock(&MY_MUTEX, 7000)) {
			pr_info("Mutex timedout task 2 \n");
			suspend_task(MYSELF);
		} else {
			pr_info("got the mutex task 2\n");

		}
		mutex_unlock(&MY_MUTEX);
#endif
		suspend_task(MYSELF);
	}
}

void task_1(void)
{
	pr_info("entering task_1\n");
	while(1) 
	{
		#if 1
		if(OS_OK != mutex_lock(&MY_MUTEX, 10000)) {
                        pr_info("Mutex timedout task 1 \n");
				suspend_task(MYSELF);
                } else {
                        pr_info("got the mutex task 1\n");

                }
		mutex_unlock(&MY_MUTEX);
		#endif
		suspend_task(MYSELF);
	}
}

void idle_task(void)
{
	int a = 100000;
	int b = 200000;
	int c = 0;

        while(1) {
            //pr_info("I ");
	    c = a + b;
        }
}

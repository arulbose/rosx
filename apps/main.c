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

/* An example Kernel timer  application */

#include <RoseRTOS.h>

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static TCB idle_tcb;
static TCB task_1_tcb;
static TCB task_2_tcb;
static TCB task_3_tcb;

struct priv {
        struct timer_list *timer;
};

void timer_callback(void *);

void timer_callback(void *priv)
{
	struct priv *my_priv = priv;

	pr_info("received timer call-back\n");
	mod_timer(my_priv->timer, 6);
}

int system_irq(int irq_num, void *a);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    create_task(&idle_tcb,"idle", LEAST_PRIO, 0, 8192, idle_task, TASK_READY, 0);	
    create_task(&task_1_tcb,"task1", 1, 0, 8192, task_1, TASK_READY, 0);	
    create_task(&task_2_tcb,"task2", 2, 0, 8192, task_2, TASK_READY, 0);	
    create_task(&task_3_tcb,"task3", 3, 0, 8192, task_3, TASK_READY, 0);	
    rose_sched();
}

void task_3(void)
{
    int count = 0;
    pr_info("Enter task_3\n");
    while(1) {
        ssleep(3);
        count ++;
        pr_info("task_3 %d\n", count);
    }
}

void task_2(void)
{
    int count = 0;
    pr_info("Enter task_2\n");
    while(1) {
        ssleep(9);
        count ++;
        pr_info("task_2 %d\n", count);
    }
}

void task_1(void)
{
    struct timer_list *timer = NULL;
    struct priv my_priv;   
 
    pr_info("Enter task_1\n");
    if(NULL == (timer = create_timer(timer_callback, &my_priv, 5))) {
		pr_panic("task_1: create_timer failed");
    }
	
    my_priv.timer = timer;

    start_timer(timer); 
    pr_info("Timer started\n");
   
    suspend_task(MYSELF); 
    while(1) {
			
		
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

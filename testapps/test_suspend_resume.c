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

/* Test suspend_resume */

#include <RoseRTOS.h>

#define LEAST_PRIO (CONFIG_MAX_TASK - 1)

void idle_task(void);
void task1(void);
void task2(void);

static TCB idle_tcb;
static TCB task1_tcb;
static TCB task2_tcb;

int system_irq(int irq_num, void *a);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    create_task(&idle_tcb,"idle", LEAST_PRIO, 0, 8192, idle_task, TASK_READY,0);	
    create_task(&task1_tcb,"task1", 2, 0, 8192, task1, TASK_READY,0);	
    create_task(&task2_tcb,"task2", 3, 0, 8192, task2, TASK_READY,0);	

	  if(OS_OK != request_irq(0, &system_irq, 0, "sys_irq", 0)) {
                pr_error("request_irq failed\n");
                        exit(1);
        }

    /* May enable interrupt before calling rose_sched() */
    rose_sched();
}

int sys_irq;

int system_irq(int irq_num, void *a)
{
	sys_irq ++;
	return IRQ_HANDLED;
}

void idle_task(void)
{
	int a = 100000;
	int b = 200000;
	int c = 0;

        while(1) {
                pr_info("I%d%d ", __raw_irq_counter, sys_irq);
		c = a + b;
                rose_sched();
        }
}


void task1(void)
{
	int a = 1000;
	int b = 1000;
	int c = 0;
	pr_info("Entering task1\n");
	suspend_task(MYSELF);
	while(1) {
		//pr_info(" T1%d ", __raw_irq_counter);
		resume_task(&task2_tcb);
		c = a + b;
		while(c--);
		suspend_task(MYSELF);
	}
}

void task2(void)
{
	int a = 1000;
	int b = 2000;
	int c = 0;
	pr_info("Entering task2\n");
 while(1) {
		//pr_info(" T2%d ", __raw_irq_counter);
                resume_task(&task1_tcb);
		c = a + b;
		while(c--);
                suspend_task(MYSELF);
        }
	
}

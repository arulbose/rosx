
/* An example Semaphore application */

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
       int num;
};

struct priv MY_PRIV = {10};
DEFINE_SEMAPHORE(s, 1);	

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    create_task(&idle_tcb,"idle", LEAST_PRIO, 0, 8192, idle_task, TASK_READY, 0);	
    create_task(&task_1_tcb,"task1", 3, 0, 8192, task_1, TASK_READY, 0);	
    create_task(&task_2_tcb,"task2", 2, 0, 8192, task_2, TASK_READY, 0);	
    create_task(&task_3_tcb,"task3", 1, 0, 8192, task_3, TASK_READY, 0);
    
    rose_sched();
}

void task_3(void)
{
    pr_info("Entering task_3\n");
	while(1) {
	        	
                semaphore_wait(&s, OS_WAIT_FOREVER);
                pr_info("Task 3 Acquired semaphore\n");
                ssleep(6);
                semaphore_post(&s);
                pr_info("Task 3 Released semaphore\n");
		suspend_task(MYSELF);
	}
}

void task_2(void)
{
	pr_info("entering task_2\n");
	while(1) {
                semaphore_wait(&s, OS_WAIT_FOREVER);
                pr_info("Task 2 Acquired semaphore\n");
                ssleep(3);
                semaphore_post(&s);
                pr_info("Task 2 Released semaphore\n");
		suspend_task(MYSELF);
	}
}

void task_1(void)
{
	pr_info("entering task_1\n");
	while(1) {
                semaphore_wait(&s, OS_WAIT_FOREVER);
                pr_info("Task 1 Acquired semaphore\n");
                ssleep(1);
                semaphore_post(&s);
                pr_info("Task 1 Released semaphore\n");
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


/* An example Mutex application */

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
DEFINE_MUTEX(m);	

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
	while(1) {
			
		mutex_lock(&m, OS_WAIT_FOREVER);
		MY_PRIV.num  = 13;
		pr_info("Acquired mutex task_3\n");
		ssleep(3);
		mutex_unlock(&m);
		pr_info("Release mutex task_3\n");
		suspend_task(MYSELF);
	}

}

void task_2(void)
{
	pr_info("entering task_2\n");
	while(1) {
		mutex_lock(&m, OS_WAIT_FOREVER);
		pr_info("Acquired mutex task_2\n");
		MY_PRIV.num  = 12;
		mutex_unlock(&m);
		pr_info("Release mutex task_2\n");
		suspend_task(MYSELF);
	}
}

void task_1(void)
{
	pr_info("entering task_1\n");
	while(1) {
		mutex_lock(&m, OS_WAIT_FOREVER);
		pr_info("Acquired mutex task_1\n");
		MY_PRIV.num  = 11;
		mutex_unlock(&m);
		pr_info("Release mutex task_1\n");
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

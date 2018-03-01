
/* An example mutex timeout */

#include <RoseRTOS.h>

/* An Example Mutex with time out application */

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
                pr_info("Mutex lock task 3 \n");
		ssleep(12);
		mutex_unlock(&m);
                pr_info("Mutex unlock task 3 \n");
		suspend_task(MYSELF);
	}

}

void task_2(void)
{
	pr_info("entering task_2\n");

	while(1) {
#if 1
		if(OS_OK != mutex_lock(&m, SECS_TO_TICKS(7) )) {
			pr_info("Mutex timedout task 2 \n");
			suspend_task(MYSELF);
		} else {
			pr_info("got the mutex task 2\n");

		}
		mutex_unlock(&m);
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
		if(OS_OK != mutex_lock(&m, SECS_TO_TICKS(10))) {
                        pr_info("Mutex timedout task 1 \n");
				suspend_task(MYSELF);
                } else {
                        pr_info("got the mutex task 1\n");

                }
		mutex_unlock(&m);
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

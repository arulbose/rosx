
/* An example mutex timeout */

#include <RosX.h>

/* An Example Mutex with time out application */

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static RX_TASK idle_tcb;
static RX_TASK task_1_tcb;
static RX_TASK task_2_tcb;
static RX_TASK task_3_tcb;

struct priv {
       int num;
};

struct priv MY_PRIV = {10};
DEFINE_MUTEX(m);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY, 0);	
    rx_create_task(&task_1_tcb,"task1", 3, 0, 8192, task_1, RX_TASK_READY, 0);	
    rx_create_task(&task_2_tcb,"task2", 2, 0, 8192, task_2, RX_TASK_READY, 0);	
    rx_create_task(&task_3_tcb,"task3", 1, 0, 8192, task_3, RX_TASK_READY, 0);
    
    rx_sched();
}

void task_3(void)
{
	while(1) {
			
		rx_mutex_lock(&m, OS_WAIT_FOREVER);
                pr_dbg("Mutex lock task 3 \n");
		rx_ssleep(12);
		rx_mutex_unlock(&m);
                pr_dbg("Mutex unlock task 3 \n");
		rx_suspend_task(MYSELF);
	}

}

void task_2(void)
{
	pr_dbg("entering task_2\n");

	while(1) {
#if 1
		if(OS_OK != rx_mutex_lock(&m, SECS_TO_TICKS(7) )) {
			pr_dbg("Mutex timedout task 2 \n");
			rx_suspend_task(MYSELF);
		} else {
			pr_dbg("got the mutex task 2\n");

		}
		rx_mutex_unlock(&m);
#endif
		rx_suspend_task(MYSELF);
	}
}

void task_1(void)
{
	pr_dbg("entering task_1\n");
	while(1) 
	{
		#if 1
		if(OS_OK != rx_mutex_lock(&m, SECS_TO_TICKS(10))) {
                        pr_dbg("Mutex timedout task 1 \n");
				rx_suspend_task(MYSELF);
                } else {
                        pr_dbg("got the mutex task 1\n");

                }
		rx_mutex_unlock(&m);
		#endif
		rx_suspend_task(MYSELF);
	}
}

void idle_task(void)
{
	int a = 100000;
	int b = 200000;
	int c = 0;

        while(1) {
            //pr_dbg("I ");
	    c = a + b;
        }
}

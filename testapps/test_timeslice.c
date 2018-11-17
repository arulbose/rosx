
/* Time slice test application */
#include <RosX.h>

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static RX_TASK idle_tcb;
static RX_TASK task_1_tcb;
static RX_TASK task_2_tcb;
static RX_TASK task_3_tcb;

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY, 0);	
    rx_create_task(&task_1_tcb,"task1", 3, 0, 8192, task_1, RX_TASK_READY, 5);	
    rx_create_task(&task_2_tcb,"task2", 3, 0, 8192, task_2, RX_TASK_READY, 10);	
    rx_create_task(&task_3_tcb,"task3", 3, 0, 8192, task_3, RX_TASK_READY, 15);
    
    rx_sched();
}

#define DELAY 1000000
void task_3(void)
{
	int a = DELAY;

	while(1) {
			while(a --);

			a = DELAY;
			pr_dbg("3 ");		
	}

}

void task_2(void)
{
	int a = DELAY;
	pr_dbg("entering task_2\n");
	while(1) {
        		while(a --);

                        a = DELAY;
                        pr_dbg("2 ");
	}
}

void task_1(void)
{
	int a = DELAY;
	pr_dbg("entering task_1\n");
	while(1) {
                       while(a --);

                        a = DELAY;
                        pr_dbg("1 ");
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

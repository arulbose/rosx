
/* Time slice test application */
#include <RoseRTOS.h>

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static TCB idle_tcb;
static TCB task_1_tcb;
static TCB task_2_tcb;
static TCB task_3_tcb;

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    create_task(&idle_tcb,"idle", LEAST_PRIO, 0, 8192, idle_task, TASK_READY, 0);	
    create_task(&task_1_tcb,"task1", 3, 0, 8192, task_1, TASK_READY, 5);	
    create_task(&task_2_tcb,"task2", 3, 0, 8192, task_2, TASK_READY, 10);	
    create_task(&task_3_tcb,"task3", 3, 0, 8192, task_3, TASK_READY, 15);
    
    rose_sched();
}

#define DELAY 1000000
void task_3(void)
{
	int a = DELAY;

	while(1) {
			while(a --);

			a = DELAY;
			pr_info("3 ");		
	}

}

void task_2(void)
{
	int a = DELAY;
	pr_info("entering task_2\n");
	while(1) {
        		while(a --);

                        a = DELAY;
                        pr_info("2 ");
	}
}

void task_1(void)
{
	int a = DELAY;
	pr_info("entering task_1\n");
	while(1) {
                       while(a --);

                        a = DELAY;
                        pr_info("1 ");
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

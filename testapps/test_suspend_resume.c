
/* Test suspend_resume */

#include <RosX.h>

#define RX_TASK_LEAST_PRIO (CONFIG_MAX_TASK - 1)

void idle_task(void);
void task1(void);
void task2(void);

static RX_TASK idle_tcb;
static RX_TASK task1_tcb;
static RX_TASK task2_tcb;

int system_irq(int irq_num, void *a);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY,0);	
    rx_create_task(&task1_tcb,"task1", 2, 0, 8192, task1, RX_TASK_READY,0);	
    rx_create_task(&task2_tcb,"task2", 3, 0, 8192, task2, RX_TASK_READY,0);	

	  if(OS_OK != request_irq(0, &system_irq, 0, "sys_irq", 0)) {
                pr_error("request_irq failed\n");
                        exit(1);
        }

    /* May enable interrupt before calling rx_sched() */
    rx_sched();
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
                rx_sched();
        }
}


void task1(void)
{
	int a = 1000;
	int b = 1000;
	int c = 0;
	pr_info("Entering task1\n");
	rx_suspend_task(MYSELF);
	while(1) {
		//pr_info(" T1%d ", __raw_irq_counter);
		resume_task(&task2_tcb);
		c = a + b;
		while(c--);
		rx_suspend_task(MYSELF);
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
                rx_suspend_task(MYSELF);
        }
	
}

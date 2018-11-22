
/* An example Kernel tasklet  application */

#include <RosX.h>

void idle_task(void);
void task1(void);

static RX_TASK idle_tcb;
static RX_TASK task_1_tcb;
void first_tasklet_handler(unsigned long data);
void second_tasklet_handler(unsigned long data);
RX_DEFINE_TASKLET(first, first_tasklet_handler, 0);
RX_DEFINE_TASKLET(second, second_tasklet_handler, 0);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void rx_application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY, 0);	
    rx_create_task(&task_1_tcb,"task1", 1, 0, 8192, task1, RX_TASK_READY, 0);	
    rx_sched();
}


void first_tasklet_handler(unsigned long data)
{
    pr_info("first_tasklet_handler running\n");
}

void second_tasklet_handler(unsigned long data)
{
    pr_info("second_tasklet_handler running\n");
}

void idle_task(void)
{
    while(1) 
    {
       // Do nothing
    }
}

void task1(void)
{
    rx_enable_tasklet(&first);
    rx_enable_tasklet(&second);
        
    while(1) 
    {
        rx_schedule_tasklet(&first);
        rx_ssleep(2);
        rx_schedule_tasklet(&second);
        rx_ssleep(1);
    }
}

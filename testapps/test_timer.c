
/* 
 * Test kernel timer
 */

#include <RosX.h>

void idle_task(void);
void task_1(void);
void task_2(void);
void task_3(void);

static RX_TASK idle_tcb;
static RX_TASK task_1_tcb;
static RX_TASK task_2_tcb;
static RX_TASK task_3_tcb;

struct priv {
        struct timer_list *timer;
};

void timer_callback(void *);

void timer_callback(void *priv)
{
	struct priv *my_priv = priv;

	pr_info("received timer call-back\n");
	rx_mod_timer(my_priv->timer, 6);
}

int system_irq(int irq_num, void *a);

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY, 0);	
    rx_create_task(&task_1_tcb,"task1", 1, 0, 8192, task_1, RX_TASK_READY, 0);	
    rx_create_task(&task_2_tcb,"task2", 2, 0, 8192, task_2, RX_TASK_READY, 0);	
    rx_create_task(&task_3_tcb,"task3", 3, 0, 8192, task_3, RX_TASK_READY, 0);	
    rx_sched();
}

void task_3(void)
{
    int count = 0;
    pr_info("Enter task_3\n");
    while(1) {
        rx_ssleep(3);
        count ++;
        pr_info("task3 %d\n", count);
    }
}

void task_2(void)
{
    int count = 0;
    pr_info("Enter task_2\n");
    while(1) {
        rx_ssleep(9);
        count ++;
        pr_info("task2 %d\n", count);
    }
}

void task_1(void)
{
    struct timer_list *timer = NULL;
    struct priv my_priv;   
 
    pr_info("Enter task_1\n");
    if(NULL == (timer = rx_create_timer(timer_callback, &my_priv, 5))) {
		pr_panic("task_1: rx_create_timer failed");
    }
	
    my_priv.timer = timer;

    rx_start_timer(timer); 
    pr_info("Timer started\n");
   
    rx_suspend_task(MYSELF); 
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

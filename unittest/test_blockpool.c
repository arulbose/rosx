
/* An example block allocation and release  application */

#include <RosX.h>

void idle_task(void);
void task_1(void);

static RX_TASK idle_tcb;
static RX_TASK task_1_tcb;
char pool[500];   

/* Application main task expected to do all required App specific initialization before enabling interrupts */
void rx_application_init(void)
{
    /* Create all application task  */
    rx_create_task(&idle_tcb,"idle", RX_TASK_LEAST_PRIO, 0, 8192, idle_task, RX_TASK_READY, 0);	
    rx_create_task(&task_1_tcb,"task1", 1, 0, 8192, task_1, RX_TASK_READY, 0);	
    rx_sched();
}

void print_pool(struct block_pool *p)
{
    pr_dbg("Name of the pool - %s\n", p->name);
    pr_dbg("Magic number - 0x%x\n", p->magic);
    pr_dbg("start_of_pool 0x%x\n", p->start_of_pool);
    pr_dbg("pool size %d\n", p->pool_size);
    pr_dbg("block size %d\n", p->block_size);
    pr_dbg("Available %d\n", p->available);
    pr_dbg("free_blocks ptr 0x%x\n", p->free_blocks);
    pr_dbg("\n");
}

void task_1(void)
{
    struct block_pool pool_data;
    int count = 0;
    char *ptr_to_block[10];
    char *t;
    pr_dbg("Enter task_1\n");

    /* Create pool */
    rx_create_block_pool(&pool_data, "mypool", 64, pool, 500);
    print_pool(&pool_data);

    /* Allocate pool */
    pr_dbg("----- Allocate pool ---- \n");
    while(OS_OK == rx_allocate_block(&pool_data, (void*) (&ptr_to_block[count]), OS_NO_WAIT))
    {
         t = *(ptr_to_block + count);
         *t = 'A';
         *(t + 1) = 'B';
         *(t + 2) = 'C';
         *(t + 3) = 'D';
         print_pool(&pool_data);
	 count++;
    }

    count --;
    pr_dbg("----- Free pool ---- \n");
    /* Free pool */
    while(count >= 0) {

	rx_release_block(ptr_to_block[count]);
	print_pool(&pool_data);
        count --; 	

    }
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
            //pr_dbg("I ");
	    c = a + b;
        }
}

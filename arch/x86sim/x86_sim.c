/* RosX RT-Kernel
 * Copyright (C) 2016 Arul Bose<bose.arul@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* RosX x86 simulator */

#include <RosX.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

struct shm_struct {
	pid_t pid;
	int num;
};

pid_t pid;
key_t mem_key;
int   shm_id;
struct shm_struct *shm_ptr;

/* to handle simulated interrupts; acts as the main interrupt entry point  */
void sig_handler(int signum)
{
    RX_TASK *tcb = __rx_curr_running_task;
    
    __context = __IRQ; 

    if (signum == SIGUSR1)
    {
        __rx_irq_handler(shm_ptr->num); /* RTOS IRQ hook; IRQ number */
    }
#ifdef CONFIG_KERNEL_PREEMPT
	/* check for re-sched and alter the stack to switch to higher priority task */
    if(__rx_need_resched) {
	__rx_need_resched = 0; /* to avoid recursion of preempt code */
	tcb->state = RX_TASK_READY;
    	__preempt__(tcb);
    }
#endif

/* Switch context before leaving the IRQ simulator */
    __context = __USER;
}

/* Signal setup for interrupt handler */
void init_signals()
{
    memset (&act, 0, sizeof(act));
    act.sa_handler = sig_handler;
 
    if (sigaction(SIGUSR1, &act, 0)) {
 	rx_early_printk("sigaction error");
	exit(1);
    }
 
    sigemptyset (&mask);
    sigaddset (&mask, SIGUSR1);
}

int main()
{
    /******* Simulator interrupt initialization<Unix signal used as asynchronus interrupt > *******/
    pid = getpid();
    /* generates an IPC key */
    mem_key   = ftok(".", 'c');
    /* Allocates a System V shared memory segment */
    if( -1 == (shm_id   = shmget(mem_key, sizeof(struct shm_struct), IPC_CREAT | 0666))) {
	rx_early_printk("Shget failed \n");
	exit(1);
    }
    /* attaches the System V shared memory segment identified by the shm_id */
    if((void *)-1 == (shm_ptr  = (struct shm_struct *) shmat(shm_id, NULL, 0))) {
	rx_early_printk("Shmat failed\n");
          exit(1);
    }

    shm_ptr->pid = pid;
    init_signals();

    /******** Port specific Rosx RTOS initialization <start> ********/

    #ifdef CONFIG_STACK_ALLOC_DYNAMIC
    __rx_stack_start_ptr = ((char *)malloc(CONFIG_SYSTEM_STACK_SIZE) + CONFIG_SYSTEM_STACK_SIZE);
    if(!__rx_stack_start_ptr) {
  	rx_early_printk("System stack allocation failed\n");
 	exit(1);
    }
    rx_early_printk("__rx_stack_start_ptr = %p\n", __rx_stack_start_ptr);
    #endif

    #ifdef CONFIG_PRINT_BUFFER
    __rx_printk_buffer_start_ptr = (char *)malloc(CONFIG_PRINT_BUFFER_SIZE);
    if(!__rx_printk_buffer_start_ptr) {
  	rx_early_printk("System print buffer allocation failed\n");
 	exit(1);
    }
    #endif 

    /******** Port specific Rosx RTOS initialization <end> ********/

    /* Entry point of Rosx RTOS  */
    __rx_kernel_enter();
    return 0;
}


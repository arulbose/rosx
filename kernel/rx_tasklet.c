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

#include <RosX.h>

struct tasklet * sys_tasklet_list = NULL;
/* Under construction */

int rx_create_tasklet(struct tasklet *t, void(*func)(unsigned long), unsigned long data)
{
    if((t == NULL) || (func == NULL) ){
        return OS_ERR;
    }

    t->status = __RX_DISABLE_TASKLET;
    t->func = func;
    t->data = data;
    t->next = NULL;
    t->prev = NULL;
  
    return OS_OK;

}

void rx_delete_tasklet(struct tasklet *t)
{

}

/* Enable the tasklet if it is disabled */
void rx_enable_tasklet(struct tasklet *t)
{
    unsigned int imask;

    if(t == NULL) {
        return OS_ERR;
    }

    imask = rx_enter_critical();
    if(t->status == __RX_DISABLE_TASKLET) {
        t->status = __RX_ENABLE_TASKLET;
    }
    rx_exit_critical(imask);

}
/* Disable the tasklet if enabled; In case 
 * it is already scheduled remove from the global task list
 * and then disable; If tasklet is running than it will complete 
 * and will get disabled */
void rx_disable_tasklet(struct tasklet *t)
{
    unsigned int imask;

    if(t == NULL) {
        return OS_ERR;
    }

    imask = rx_enter_critical();
    if(t->status != __RX_DISABLE_TASKLET){
        if(t->status & __RX_SCHED_TASKLET) {
          /* Remove the task from the global tasklet list */
        }
        /* If the tasklet is already running then it will run and gets disabled */
        t->status = __RX_DISABLE_TASKLET;
    }
    rx_exit_critical(imask);

}

/* 1. When called schedule tasklet will be added to the global system tasklist to be processed
   2. Tasklet can be schedule multiple times but runs only once if it is not already running.
   3. A schedule call when a taslet is already running will run again after completion
   4. If the tasklet is disabled while running it will complete its run and then gets disabled
*/
void rx_schedule_tasklet(struct tasklet *t)
{
    unsigned int imask;

    if(t == NULL) {
        return OS_ERR;
    }

    imask = rx_enter_critical();
    if(t->status & __RX_SCHED_TASKLET ) {
        /* Already scheduled; just return*/
        return;
    }

    /* Add the taslket to the global tasklet list */
    t->status |= __RX_SCHED_TASKLET;
    if(!sys_tasklet_list) {
	
    }else{

    }
	  
    rx_exit_critical(imask);

}

void rx_bh_thread()
{
    RX_DEFINE_WAITQUEUE(w);

    while(1) {

         if(0 == rx_wait_queue(&w, (sys_tasklet_list != NULL))) {
         /* Process the tasklet serially */


         }

    }
}

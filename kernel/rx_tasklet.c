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
  
    return OS_OK;

}

void rx_delete_tasklet(struct tasklet *t)
{

}

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

void rx_disable_tasklet(struct tasklet *t)
{
    unsigned int imask;

    if(t == NULL) {
        return OS_ERR;
    }

    imask = rx_enter_critical();
    if(t->status != __RX_DISABLE_TASKLET){
        if(t->status == __RX_SCHED_TASKLET) {
          /* Remove the task from the global tasklet list */
        }
        /* If the task is already running then it will run and gets disabled */
        t->status == __RX_DISABLE_TASKLET;
    }
    rx_exit_critical(imask);

}

void rx_schedule_tasklet(struct tasklet *t)
{
    unsigned int imask;

    if(t == NULL) {
        return OS_ERR;
    }

    imask = rx_enter_critical();
    if(t->status != __RX_SCHED_TASKLET) {
        t->status = __RX_SCHED_TASKLET;
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

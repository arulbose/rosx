/* Rosx RT-Kernel
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

static RX_TASK rx_timer0_tcb;
static RX_TASK rx_event0_tcb;
#ifdef CONFIG_PRINT_BUFFER
static RX_TASK rx_logger0_tcb;
#endif
static RX_TASK rx_terminal0_tcb;
static RX_TASK rx_bh0_tcb;

static char sys_timer_stack_ptr[CONFIG_SYS_TIMER_STACK_SIZE];
static char sys_event_stack_ptr[CONFIG_SYS_EVENT_STACK_SIZE];
static char sys_bh_stack_ptr[CONFIG_SYS_BH_STACK_SIZE];
static char sys_terminal_stack_ptr[CONFIG_SYS_TERMINAL_STACK_SIZE];
#ifdef CONFIG_PRINT_BUFFER
static unsigned char sys_logger_stack_ptr[CONFIG_SYS_LOGGER_STACK_SIZE];
#endif

/* App entry point */
extern void rx_application_init(void);

/* First OS call made after platform initialization 
*  All OS specific initialization done here 
*/
void __rx_kernel_enter()
{
    /* stack init */
    __rx_curr_stack_ptr = __rx_stack_start_ptr; 
    /* init task(just a place holder needed for the first context switch ) */
    strcpy(__rx_init_task.name, "init"); 
    __rx_curr_running_task = &__rx_init_task;
    __rx_curr_num_task = 0;

    /* driver initialization */
    rx_driver_init();

    /* create all system threads */
    rx_create_task(&rx_timer0_tcb,"timer0", RX_TASK_HIGH_PRIO, &sys_timer_stack_ptr, CONFIG_SYS_TIMER_STACK_SIZE, rx_timer_thread, RX_TASK_READY, 0); /* system timer thread */
    rx_create_task(&rx_event0_tcb,"event0", RX_TASK_HIGH_PRIO, &sys_event_stack_ptr, CONFIG_SYS_EVENT_STACK_SIZE, rx_event_thread, RX_TASK_READY, 0); /* system event thread */
    rx_create_task(&rx_bh0_tcb,"bh0", RX_TASK_HIGH_PRIO, &sys_bh_stack_ptr, CONFIG_SYS_BH_STACK_SIZE, rx_bh_thread, RX_TASK_READY, 0); /* bottom half thread */
            
#ifdef CONFIG_PRINT_BUFFER
    /* Logger init */
    __rx_printk_buffer_head = __rx_printk_buffer_tail = __rx_printk_buffer_start_ptr;
    rx_create_task(&rx_logger0_tcb,"logger0", (RX_TASK_LEAST_PRIO - 1), &sys_logger_stack_ptr, CONFIG_SYS_LOGGER_STACK_SIZE, rx_logger_thread, RX_TASK_READY, 0); /* system logger thread */
#endif

    /* terminal */
    rx_create_task(&rx_terminal0_tcb,"term0", (RX_TASK_LEAST_PRIO - 2), &sys_terminal_stack_ptr, CONFIG_SYS_TERMINAL_STACK_SIZE, rx_terminal_thread, RX_TASK_READY, 0); /* terminal emulator thread */
    
    rx_application_init();	
    /* NO RETURN */
}


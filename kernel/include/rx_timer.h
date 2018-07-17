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

#ifndef __RX_TIMER_H__
#define __RX_TIMER_H__

/********************** timers ****************************/
#define RX_TIMER_EVENT_IRQ_EVENT (1 << 0)

/* doubley link-list */
struct timer_list {
        unsigned int time_expiry; /* delay + jiffies */
	unsigned int delay; /* actual delay */
	int flag;
        void (* handler)(void *);
	void *priv;
        RX_TASK *task;
        struct timer_list *next;
        struct timer_list *prev;
	};
/* timer interrupt */
int __rx_timer_tick_irq_handler(int irq, void *a);

#ifdef LOG_DBG
/* Debug data */
int rx_g_timer_pool_used;
#endif

/* Controlled by start/stop timer */
#define __RX_TIMER_ENABLED (1 << 0)
#define __RX_TIMER_DISABLED ~(__RX_TIMER_ENABLED)

volatile unsigned int jiffies;
struct event_group __rx_timer_events;
struct timer_list *__rx_active_timer_head;

/* Internal functions */
void rx_timer_thread(void);
int __rx_tick_timer(void);
int __rx_add_timer(struct timer_list *, void (*func)(void *), int ticks, RX_TASK *tid);
void __rx_add_to_active_timer_list(struct timer_list *p);
void __rx_remove_from_timer_list(struct timer_list *p, struct timer_list **head);

/* -------------- Application system calls ---------------- */
int rx_init_timer(struct timer_list *timer, void (*timer_handler)(void *), void *priv, unsigned int delay);
int rx_create_timer(struct timer_list *, void (*timer_handler)(void *), void *priv, unsigned int ticks);
void rx_delete_timer(struct timer_list *p);
void rx_start_timer(struct timer_list *p);
void rx_stop_timer(struct timer_list *p);
void rx_mod_timer(struct timer_list *p, unsigned int delay);
void rx_ndelay();
void rx_udelay();
void rx_mdelay();
int rx_msleep(unsigned int msecs);
int rx_ssleep(unsigned int secs);

/* Handy inlines*/
//inline int MSECS_TO_TICKS(int m);
/* Handy defines */
#define RX_TICKS_TO_MS(ticks) (((ticks) * (1000/CONFIG_HZ)))
#define RX_TICKS_TO_SECS(ticks) (TICKS_TO_MS(ticks) * 1000)
#define RX_SECS_TO_TICKS(s)  ((s) * (1000/CONFIG_HZ))

#define __RX_TIMER_OFF 0
#define __RX_TIMER_ON  1

#endif /* __TIMER_H__ */

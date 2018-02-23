/* Rose RT-Kernel
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

#ifndef __TIMER_H__
#define __TIMER_H__

/********************** timers ****************************/
#define TIMER_EVENT_IRQ_EVENT (1 << 0)

/* doubley link-list */
struct timer_list {
        unsigned int time_expiry; /* delay + jiffies */
	unsigned int delay; /* actual delay */
	int flag;
        void (* handler)(void *);
	void *priv;
        TCB *task;
        struct timer_list *next;
        struct timer_list *prev;
	};
/* timer interrupt */
int timer_tick_irq_handler(int irq, void *a);

#ifdef LOG_DBG
/* Debug data */
int g_timer_pool_used;
#endif

/* Controlled by start/stop timer */
#define __TIMER_ENABLED (1 << 0)
#define __TIMER_DISABLED ~(__TIMER_ENABLED)

volatile unsigned int jiffies;
struct event_group timer_events;
struct timer_list *active_timer_head;

/* Internal functions */
void rose_timer_thread(void);
int tick_timer(void);
int __add_timer(struct timer_list *, void (*func)(void *), int ticks, TCB *tid);
void add_to_active_timer_list(struct timer_list *p);
void remove_from_timer_list(struct timer_list *p, struct timer_list **head);

/* -------------- Application system calls ---------------- */
int init_timer(struct timer_list *timer, void (*timer_handler)(void *), void *priv, unsigned int delay);
#if(CONFIG_TIMER_COUNT > 0)
struct timer_list * create_timer(void (*timer_handler)(void *), void *priv, unsigned int ticks);
void delete_timer(struct timer_list *p);
#endif
void start_timer(struct timer_list *p);
void stop_timer(struct timer_list *p);
void mod_timer(struct timer_list *p, unsigned int delay);
void ndelay();
void udelay();
void mdelay();
int msleep(unsigned int msecs);
int ssleep(unsigned int secs);

/* Handy inlines*/
inline int MSECS_TO_TICKS(int m);
/* Handy defines */
#define TICKS_TO_MS(ticks) (((ticks) * (1000/CONFIG_HZ)))
#define TICKS_TO_SECS(ticks) (TICKS_TO_MS(ticks) * 1000)
#define SECS_TO_TICKS(s)  ((s) * (1000/CONFIG_HZ))

#define __TIMER_OFF 0
#define __TIMER_ON  1

#endif /* __TIMER_H__ */

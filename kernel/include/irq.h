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

#ifndef __IRQ_H__
#define __IRQ_H__
/* IRQ */
#define IRQF_TRIGGER_EDGED     (1 << 0)
#define IRQF_TRIGGER_LEVEL     ~IRQF_TRIGGER_EDGED

#define IRQF_TRIGGER_RISING     (1 << 1)
#define IRQF_TRIGGER_FALLING    ~IRQF_TRIGGER_RISING

#define IRQF_TRIGGER_HIGH       (1 << 1)
#define IRQF_TRIGGER_LOW        ~IRQF_TRIGGER_HIGH

#define IRQF_SHARED             (1 << 2)

#define IRQ_HANDLED 0

volatile unsigned int __raw_irq_counter; /* global irq counter */

struct irq{
        char devname[8];
        int (*irq_handler)(int irq, void *dev_id);
        int flags;
        void *dev_id;
        struct irq *next; /* for shared irq */
};

/* Kernel entry point interrupt handler */
void __irq_handler(int irq_num);
#define enable_irq(x)  __enable_irq(x)
#define disable_irq(x)  __disable_irq(x)
#define enter_critical(x)  __disable_irq_global(x)
#define exit_critical(x) __restore_irq_global(x)

/* -------------- Application system calls ---------------- */
int request_irq(int irq_num, int (*irq_handler)(int, void *), unsigned int irq_flags, char *dev_name, void *dev_id);
int free_irq(int irq_num, void *p);
#endif

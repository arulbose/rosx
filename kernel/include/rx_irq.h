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

#ifndef __IRQ_H__
#define __IRQ_H__
/* IRQ */
#define RX_IRQF_TRIGGER_EDGED     (1 << 0)
#define RX_IRQF_TRIGGER_LEVEL     ~RX_IRQF_TRIGGER_EDGED

#define RX_IRQF_TRIGGER_RISING     (1 << 1)
#define RX_IRQF_TRIGGER_FALLING    ~RX_IRQF_TRIGGER_RISING

#define RX_IRQF_TRIGGER_HIGH       (1 << 1)
#define RX_IRQF_TRIGGER_LOW        ~RX_IRQF_TRIGGER_HIGH

#define RX_IRQF_SHARED             (1 << 2)

#define RX_IRQ_HANDLED 0

volatile unsigned int __rx_raw_irq_counter; /* global irq counter */

struct irq{
        char devname[8];
        int (*irq_handler)(int irq, void *dev_id);
        int flags;
        void *dev_id;
        struct irq *next; /* for shared irq */
};

/* Kernel entry point interrupt handler */
void __rx_irq_handler(int irq_num);
#define rx_enable_irq(x)  __enable_irq(x)
#define rx_disable_irq(x)  __disable_irq(x)
#define rx_enter_critical(x)  __disable_irq_global(x)
#define rx_exit_critical(x) __restore_irq_global(x)

/* -------------- Application system calls ---------------- */
int rx_request_irq(int irq_num, int (*irq_handler)(int, void *), unsigned int irq_flags, char *dev_name, void *dev_id);
int rx_free_irq(int irq_num, void *p);
#endif

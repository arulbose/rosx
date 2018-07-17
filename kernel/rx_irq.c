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

#include <RosX.h>

struct irq __rx_irq_pool[CONFIG_IRQ_COUNT - 1];

/** __irq_handler(): Called by port interrupt code
 *  @irq_num:        Interrupt number as received from the interrupt controllers
 */

void __rx_irq_handler(int irq_num)
{
    //struct irq *p = NULL;

    __rx_raw_irq_counter ++;

    /* add code for mapping the Kernel irq line with the physical line */
    if( (__rx_irq_pool[irq_num].irq_handler != NULL) || (__rx_irq_pool[irq_num].next != NULL)) {
        __rx_irq_pool[irq_num].irq_handler(irq_num, __rx_irq_pool[irq_num].dev_id);
    #if 0
        /* Invoke if any shared handlers */
        if(__rx_irq_pool[irq_num].flags & RX_IRQF_SHARED){
            p = __rx_irq_pool[irq_num].next;
	    /* go through all the shared irq handler */
	    while(p != NULL){
	        p->irq_handler(irq_num, p->dev_id);
	        p = p->next;
	    }
        }
    #endif
    }
}

/** request_irq() - Install interrupt handler
 *  @irq_num:       Interrupt number from interrupt controller
 *  @irq_handler:   Handler installed by request_irq()
 *  @irq_flags:     Interrupt flags
 *  @dev_name:      Name of the driver using the interrupt 
 *  @dev_id:        Unique pointer to pass to interrupt handlers, used as a signature for shared interrupt handlers
 */
int rx_request_irq(int irq_num, int (*irq_handler)(int, void*), unsigned int irq_flags, char *dev_name, void *dev_id)
{
	//struct irq *p =  NULL;

	if(irq_num > CONFIG_IRQ_COUNT)
		return OS_ERR;
	/* add code for mapping RT irq line with physical irq line */

	/* check if irq is already allocated if not check if the request is for shared irq */
	 if(__rx_irq_pool[irq_num].irq_handler != NULL || __rx_irq_pool[irq_num].next != NULL) {
	    /* Only shared handler can have more than one irq handler */
	    if(!(irq_flags & RX_IRQF_SHARED) || !(dev_id)) /* Dev_id cannot be empty in shared handlers */
		return E_OS_UNAVAIL;

    #if 0
	    p = __rx_irq_pool[irq_num].next;
	    while(p != NULL)
                p = p->next;

	    if(NULL == (p = (struct irq *)__rx_alloc_pool(RX_SHIRQ_POOL)))
                return E_OS_UNAVAIL;
 
	    p->irq_handler = irq_handler;
	    p->flags = irq_flags;
	    strncpy(p->devname, dev_name, 7);
	    p->dev_id = dev_id;
	    p->next = NULL;
    #endif
	}else{
	/* Init irq structure */
	    __rx_irq_pool[irq_num].irq_handler = irq_handler;
            __rx_irq_pool[irq_num].flags = irq_flags;
	   strncpy(__rx_irq_pool[irq_num].devname, dev_name, 7);
	   __rx_irq_pool[irq_num].dev_id = dev_id;
	   __rx_irq_pool[irq_num].next = NULL;

	}

	return OS_OK;
}

/** @free_irq(): Install interrupt handler
 *  @irq_num:    Interrupt number from interrupt controller
 *  @dev_id:     Unique pointer used as a signature to free shared interrupt handlers
 */
int rx_free_irq(int irq_num, void *dev_id)
{
    //struct irq *prev = NULL;
    //struct irq *curr = NULL;

    if(irq_num > CONFIG_IRQ_COUNT)
        return E_OS_UNAVAIL;
    /* Check if shared interrupts */
    if(__rx_irq_pool[irq_num].flags & RX_IRQF_SHARED) {
    #if 0		
		if(__rx_irq_pool[irq_num].dev_id == dev_id) {

			__rx_irq_pool[irq_num].irq_handler = NULL;
		}else{
		    prev = curr = __rx_irq_pool[irq_num].next;
                    while(curr != NULL) {
			if(curr->dev_id == dev_id) {
				curr->irq_handler = NULL;
				prev->next = curr->next;
				__rx_free_pool(curr, RX_SHIRQ_POOL);
				break;	
			}
                        curr = curr->next;
		    }
		}
		/* do when all sh_irq are freed */
		if(__rx_irq_pool[irq_num].next == NULL) {
		    __rx_irq_pool[irq_num].flags = 0;
		}
    #endif
    } else {

	__rx_irq_pool[irq_num].irq_handler = NULL;
    }
	return OS_OK;
}


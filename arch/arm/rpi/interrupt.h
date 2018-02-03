/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt handling.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <irq.h>

void __enable_irq_global(void);
unsigned int __disable_irq_global(void);
void __restore_irq_global(unsigned int);
/* IRQ enable disable */
void __enable_irq(unsigned int);
void __disable_irq(unsigned int);

#define __CONFIG_IRQS 1
/* Number of IRQs shared between the GPU and ARM. These correspond to the IRQs
 * that show up in the IRQ_pending_1 and IRQ_pending_2 registers.  */
#define BCM2835_NUM_GPU_SHARED_IRQS     64

/* Number of ARM-specific IRQs. These correspond to IRQs that show up in the
 * first 8 bits of IRQ_basic_pending.  */
#define BCM2835_NUM_ARM_SPECIFIC_IRQS   8

/* Total number of IRQs on this hardware.  */
#define BCM2835_NUM_IRQS (BCM2835_NUM_GPU_SHARED_IRQS + BCM2835_NUM_ARM_SPECIFIC_IRQS)
#define __CONFIG_IRQ_COUNT BCM2835_NUM_IRQS
/* Include IRQ definitions  */
#include "bcm2835.h"

#endif /* _INTERRUPT_H_ */

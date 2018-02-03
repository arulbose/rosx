/**
 * @file dispatch.c
 *
 * This file contains code that interfaces with the interrupt controller of the
 * BCM2835 SoC used in the Raspberry Pi.
 *
 * This "interrupt controller" is, more specifically, the interrupt controller
 * used by the ARM processor on the BCM2835.  That is, this interrupt controller
 * controls which IRQs the ARM receives.  The VideoCore co-processor likely has
 * its own (undocumented) way to control IRQs to itself.
 *
 * See http://xinu-os.org/BCM2835_Interrupt_Controller for additional
 * information.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <stddef.h>
#include "bcm2835.h"
#include <irq.h>

/** Layout of the BCM2835 interrupt controller's registers. */
struct bcm2835_interrupt_registers {
    uint IRQ_basic_pending;
    uint IRQ_pending_1;
    uint IRQ_pending_2;
    uint FIQ_control;
    uint Enable_IRQs_1;
    uint Enable_IRQs_2;
    uint Enable_Basic_IRQs;
    uint Disable_IRQs_1;
    uint Disable_IRQs_2;
    uint Disable_Basic_IRQs;
};

static volatile struct bcm2835_interrupt_registers * const regs =
        (volatile struct bcm2835_interrupt_registers*)INTERRUPT_REGS_BASE;

/** Bitwise table of IRQs that have been enabled on the ARM. */
static uint arm_enabled_irqs[3];

/* Check if the pending bit for an IRQ line is set; if so, call the handler
 * function.  */
static void check_irq_pending(uchar irq_num)
{
    bool handle = FALSE;

    /* Check the appropriate hardware register, depending on the IRQ number.  */
    if (irq_num >= 64)
    {
        if (regs->IRQ_basic_pending & (1 << (irq_num - 64)))
        {
            handle = TRUE;
        }
    }
    else if (irq_num >= 32)
    {
        if (regs->IRQ_pending_2 & (1 << (irq_num - 32)))
        {
            handle = TRUE;
        }
    }
    else
    {
        if (regs->IRQ_pending_1 & (1 << irq_num))
        {
            handle = TRUE;
        }
    }
    if (handle)
    {
        __irq_handler(irq_num); 
        /* The pending bit should have been cleared in a device-specific way by
         * the handler function.  As far as we can tell, it cannot be cleared
         * directly through the interrupt controller.  */
    }
}

/* Find index of first set bit in a nonzero word. */
static inline ulong first_set_bit(ulong word)
{
    return 31 - __builtin_clz(word);
}

/**
 * Processes all pending interrupt requests.
 *
 * On the BCM2835 (Raspberry Pi), this done by iterating through all registered
 * interrupts on the ARM and checking whether each one is pending.  This is not
 * necessarily the fastest way to do it, but this should minimize problems with
 * the poorly-documented hardware and conflicts with the GPU.
 */
void dispatch(void)
{
    uint i;

    for (i = 0; i < 3; i++)
    {
        uint mask = arm_enabled_irqs[i];
        while (mask != 0)
        {
            uint bit = first_set_bit(mask);
            mask ^= (1 << bit);
            check_irq_pending(bit + (i << 5));
        }
    }
}

/**
 * Enable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to enable, which must be valid on the current
 *      platform.
 */
void __enable_irq(irqmask irq_num)
{
    if (irq_num < 32)
    {
        regs->Enable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] |= 1 << irq_num;
    }
    else if (irq_num < 64)
    {
        regs->Enable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] |= 1 << (irq_num - 32);
    }
    else
    {
        regs->Enable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] |= 1 << (irq_num - 64);
    }
}

/**
 * Disable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to disable, which must be valid on the current
 *      platform.
 */
void __disable_irq(irqmask irq_num)
{
    if (irq_num < 32)
    {
        regs->Disable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] &= ~(1 << irq_num);
    }
    else if (irq_num < 64)
    {
        regs->Disable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] &= ~(1 << (irq_num - 32));
    }
    else
    {
        regs->Disable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] &= ~(1 << (irq_num - 64));
    }
}

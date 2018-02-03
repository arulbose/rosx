#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_


void __enable_irq_global(void);
unsigned int __disable_irq_global(void);
void __restore_irq_global(unsigned int);
/* IRQ enable disable */
void __enable_irq(unsigned int);
void __disable_irq(unsigned int);

#define __CONFIG_IRQS 1
#define __CONFIG_IRQ_COUNT 10

#endif /* _INTERRUPT_H_ */

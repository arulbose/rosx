ccflags-y +=-I$(srctree)/include/rosekernel
ccflags-y +=-I$(srctree)/include
ccflags-y +=-I$(srctree)/arch/$(ARCH)/include
ccflags-y +=-I$(srctree)/arch/$(ARCH)/platforms/$(strip $(CONFIG_PLATFORM))

obj-y = rx_task.o rx_init.o rx_sched.o rx_irq.o 
obj-$(CONFIG_SEMAPHORE) += rx_semaphore.o
obj-$(CONFIG_MUTEX) += rx_mutex.o
obj-$(CONFIG_QUEUE) += rx_queue.o
obj-$(CONFIG_WAIT_QUEUE) += rx_wait.o
obj-$(CONFIG_EVENTS) += rx_event.o
obj-$(CONFIG_TIMERS) += rx_timer.o
obj-$(CONFIG_SYS_BH) += rx_tasklet.o
obj-$(CONFIG_BLOCKPOOL) += rx_blockpool.o

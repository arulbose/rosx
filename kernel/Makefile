ccflags-y +=-I$(srctree)/include/rosekernel
ccflags-y +=-I$(srctree)/include
ccflags-y +=-I$(srctree)/arch/$(ARCH)/include
ccflags-y +=-I$(srctree)/arch/$(ARCH)/platforms/$(strip $(CONFIG_PLATFORM))

obj-y = task.o init.o sched.o irq.o 
obj-$(CONFIG_EVENTS) += event.o
obj-$(CONFIG_MEMPOOL) += mempool.o
obj-$(CONFIG_MUTEX) += mutex.o
obj-$(CONFIG_SEMAPHORE) += semaphore.o
obj-$(CONFIG_TIMERS) += timer.o
obj-$(CONFIG_QUEUE) += queue.o

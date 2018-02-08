ROSE - Real Time Operating System Experiment

ROSE has a tiny preemptive kernel which supports

1. Dynamic task management
2. Kernel timers
3. Mutex
4. Semaphores
5. Events
6. Memory byte/block pool
7. Message queues
8. Interrupts

I have currently added support to test the kernel on a x86 environment using macro CONFIG_X86_SIMULATOR. In simulator mode it helps to easily fix kernel bugs.

Below are the thing in my TODO list :
1. Kernel API documentation.
2. Test tool for the Kernel(there are some unit test cases in testapps/ but not complete enough)
3. Logger thread which read a common print buffer
3. terminal emulator for x86 simulator

Folder structure :

apps/ - The actual application using the kernel APIs
arch/ - Arch specific code
kernel/ - Rose kernel files
kernel/include - Rose kernel include files
lib/ - C library routines
out/ - obj files for the build
scripts/ - Build scripts
testapps/ - Kernel test applications
include/ - Common include files

X86 SIMULATOR:

To build a binary for X86 simulator run 'make -f scripts/Makefile.x86'. Add the application files in the apps/ folder and build the kernel. An exceutable named 'rosex86' will be created.
Also the simulator needs system ticks which can be generated using system_clk_100ms or system_clk_10ms executable files generated using scripts/send_signal.c. After running the kernel the tick executable should be run in another shell(in the same folder as 'rosex86')

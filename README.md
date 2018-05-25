# ROSE - Real Time Operating System Experiment

## Introduction
  I have tried to write a RTOS based on my experience and understanding on OS concepts. I just started for fun and also to keep myself busy. As the title says it is an 'experiment' Rose might not be even close to any of the successful RTOS out there. Nevertheless I have tried to make the kernel simple to understand as well added kernel services of what any decent RTOS should have. The development is on-going and would appreciate your contribution to ROSE. Please do mail me if you would like to collabrate in this project. Email: bose.arul@gmail.com

Below are the services supported by the kernel.

##### 1. Rose Scheduler
      Preemptive             : Threads with lower priority are preempted in favour of higher priority threads
      Round robin scheduling : Same priority threads are atleast executed for the time slice given.
      Priority inheritance   : Implemented as part of mutex services to avoid priority inversion
##### 2. Dynamic thread management
      Threads can be created either during initialization or execution context of other threads. There is no limit to thread creation and is limited to only by the resources(memory) available.
##### 3. Application kernel timers services
      Supports static or dynamic creation of timers. Dynamic management of timers.
##### 4. Mutex services
      Supports static or dynamic creation of Mutex.
      Supports Priority inheritance to avoid priority inversion.
      Supports Mutex timeout
##### 5. Counting semaphores
      Supports static or dynamic creation of Semaphores.
##### 6. Event Groups
      Supports static or dynamic creation of event groups
      Inter-thread event notification and synchronization between group of threads.
      Participating threads can use 32 event flags at the same time.
##### 7. Wait Queues and Wake services
      Supports static or dynamic creation of wait queues.
      Threads can wait(sleep) on queues till a given condition is met or can wait till it is waked up by another thread.
      Waitqueue condition with timeout
##### 8. Memory bytepool
      Time sensitive memory allocation using fixed memory blocks.
      Application can dynamically request  and free memory block from 1 byte to 4096 bytes.
##### 9. Mempool
      Used by kernel services to dynamically create kernel services structures to meet real time constraints(not available for applications)
##### 10. Message queues
      Used for passing messages between threads
      Supports fixed size messages. Message sizes are 1, 2, 4, 8, and 16 32-bit words
      Support dynamic creation of message queues
##### 11. Interrupt management service
      Dynamic request and free of interrupt. 
      Supports interrupt sharing
##### 12. Device driver management
      Driver core support Linux like open/release/read/write/ioctl/poll api calls
      
## Getting Started
Currently the kernel is not ported to any hardware. There are some porting code in the source for ARM Raspberry pi but not complete. I have ported the kernel to work on a 32-bit x86 simulator which runs on a x86 or x86_64 bit machine. The x86 simulator runs the Rose kernel threads as if running on a actual bare metal hardware(where the rose scheduler handles the application threads). As the priority is to stabilize the kernel the x86 simulator environment gives greater flexibility in debugging kernel using gdb.

### Prerequisites for running the rose kernel in the x86 simulator mode
Any Ubuntu, Debain, Fedora or any other Linux flavor should be able to build the kernel. I personally use Debain machine. If you are running on x86_64 please install libc 32-bit (/lib32/libc.so.6). Below are the library dependencies for the rose kernel to be built. `rosex86` is the Rose kernel binary which has the application threads as well the x86 simulator.
```
$ ldd rosex86
	linux-gate.so.1 (0xf77b3000)
	libc.so.6 => /lib32/libc.so.6 (0xf75fc000)
	/lib/ld-linux.so.2 (0xf77b6000)
```

### Folder structure
```
apps/          - The actual application using the kernel APIs
arch/          - Arch specific code
kernel/        - Rose kernel files
kernel/include - Rose kernel include files
drivers/       - All device drivers
include/       - Common include files
lib/           - Common C library routines
scripts/       - Build scripts
testapps/      - Kernel test applications
utils/         - Utilties for test or debugging
out/           - obj files for the build
```

### Building the rose kernel in the x86 simulator mode
```
git clone https://github.com/arulbose/rose.git
cd rose
make -f scripts/Makefile.x86 
```
thats it. It builds a executable named `rosex86` which has the kernel, applications and x86 simualtor port built-in.

To run follow the below steps

##Terminal 1
```
cd rose/
./rosex86
*************************************************************** 
*********** ROSE Real Time operating system experiment v0_x ***** 
***************************************************************
< The below are the application thread prints >
```
In the x86 simulator mode fake interrupts are sent to the kernel interrupt management service using Linux signals. To run the Rose kernel, 'rosex86' process should be fed with clock ticks. There are 2 clock tick binaries in rose/utils/system_clk_100ms(100 ms ticks) and rose/utils/system_clk_10ms(10ms ticks). 

Open Terminal 2 and copy the tick executable(system_clk_100ms) to rose/ folder and run system_clk_100ms from rose/ after executing 'rosex86'

##Terminal 2
```
cd rose
cp utils/system_clk_100ms .
./system_clk_100ms
```
Now you should see ROSE application prints on terminal 1

## Running the tests
All application files should be in rose/apps folder. There is a reference application named main.c in rose/apps folder. The apps/rose_defines.h file has the kernel configuration for the application threads. You can write you own application files with a different kernel configuration. Make sure to add the application file names in to makefile scripts/Makefile.x86. The other test reference applications c files are present in rose/testapps.


## Contributing
Please contact bose.arul@gmail.com

## Versioning


## Authors
* Arul Bose (bose.arul@gmail.com)


## License
This project is licensed under GNU GENERAL PUBLIC LICENSE Version 3 - see the [LICENSE.md](LICENSE.md) file for details


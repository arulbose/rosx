# RosX - Real Time Operating System Experiment

## Introduction
  I have tried to write a RTOS based on my experience and understanding on OS concepts. I just started for fun and also to keep myself busy. I have tried to make the kernel simple to understand as well added kernel services of what any decent RTOS should have. The development is on-going and would appreciate your contribution to RosX. Please do mail me if you would like to collabrate in this project. Email: bose.arul@gmail.com

Below are the services supported by the kernel.

##### 1. RosX Scheduler
      Preemptive             : Threads with lower priority are preempted in favour of higher priority threads
      Round robin scheduling : Same priority threads are atleast executed for the time slice given.
      Priority inheritance   : Implemented as part of mutex services to avoid priority inversion
##### 2. Dynamic task management
      Tasks can be created either during initialization or execution context of other tasks. There is no limit to number of task creation and is limited to only by the resources(memory) available. Dynamic and static stack allocation for tasks supported.
##### 3. Application kernel timers services
      Supports static or dynamic creation of timers. Dynamic management of timers.
##### 4. Mutex services
      Supports any number of static or dynamic creation of Mutex.
      Supports Priority inheritance to avoid priority inversion.
      Supports Mutex timeout
##### 5. Counting semaphores
      Supports any number of static or dynamic creation of Semaphores.
      Supports sempahore timeout 
##### 6. Event Groups
      Supports any number of static or dynamic creation of event groups
      Inter-thread event notification and synchronization between group of threads.
      Participating threads can use 32 event flags at the same time.
##### 7. Wait Queues and Wake services
      Supports any number of static or dynamic creation of wait queues.
      Threads can wait(sleep) on queues till a given condition is met or can wait till it is waked up by another thread.
      Waitqueue condition with timeout
##### 8. Message queues
      Used for passing messages between threads
      Supports fixed size messages. Message sizes are 1, 2, 4, 8, and 16 32-bit words
      Support dynamic creation of message queues
##### 9. Block pools
      User can create multiple pools of fixed-sized memory blocks.
      Blocks are allocated and freed in a fast and deterministic manner.
      RosX does not place any restriction on the number of blocks in a pool 
##### 10. Interrupt management service
      Dynamic request and free of interrupt. 
      Supports interrupt sharing
##### 11. Device driver management
      Driver core support Linux like open/release/read/write/ioctl/poll api calls
      
## Getting Started
Currently the kernel is not ported to any hardware. There are some porting code in the source for ARM Raspberry pi but not complete. I have ported the kernel to work on a 32-bit x86 simulator which runs on a x86 or x86_64 bit machine. The x86 simulator runs the RosX kernel threads as if running on a actual bare metal hardware(where the RosX scheduler handles the application threads). As the priority is to stabilize the kernel the x86 simulator environment gives greater flexibility in debugging kernel using gdb.

### Prerequisites for running the RosX kernel in the x86 simulator mode
Any Ubuntu, Debain, Fedora or any other Linux flavor should be able to build the kernel. I personally use Debain machine. If you are running on x86_64 please install libc 32-bit. Below are the library dependencies for the RosX kernel to be built. `RosXx86` is the RosX kernel binary which has the application threads as well the x86 simulator.
```
# apt-get install libc6-i386
# apt-get install libc6-dev-i386
```

### Folder structure
```
apps/          - The actual application using the kernel APIs
arch/          - Arch specific code
kernel/        - RosX kernel files
kernel/include - RosX kernel include files
drivers/       - All device drivers
include/       - Common include files
lib/           - Common C library routines
scripts/       - Build scripts
testapps/      - Kernel test applications
utils/         - Utilties for test or debugging
out/           - obj files for the build
```

### Building the RosX kernel in the x86 simulator mode
```
git clone https://github.com/arulbose/rosx.git
cd rosx
make -f scripts/Makefile.x86 
```
thats it. It builds a executable named `rosx86` which has the kernel, applications and x86 simualtor port built-in.

To run follow the below steps

### Terminal 1
```
cd rosx/
./rosx86
*************************************************************** 
*********** ROSX Real Time operating system experiment v0_x ***** 
***************************************************************
< The below are the application thread prints >
```
In the x86 simulator mode fake interrupts are sent to the kernel interrupt management service using Linux signals. To run the RosX kernel, 'rosx86' process should be fed with clock ticks. There are 2 clock tick binaries in rosx/utils/system_clk_100ms(100 ms ticks) and rosx/utils/system_clk_10ms(10ms ticks). 

Now open Terminal 2 and run the below

### Terminal 2
```
cd rosx/utils
./system_clk_100ms
```
Now you should see ROSX application prints on terminal 1

## Running the tests
All application files should be in rosx/apps folder. There is a reference application named main.c in rosx/apps folder. The apps/rosx_defines.h file has the kernel configuration for the application threads. You can write you own application files with a different kernel configuration. Make sure to add the application file names in to makefile scripts/Makefile.x86. The other test reference applications c files are present in rosx/testapps.


## Contributing
Please contact bose.arul@gmail.com

## Versioning


## Authors
* Arul Bose (bose.arul@gmail.com)


## License
This project is licensed under GNU GENERAL PUBLIC LICENSE Version 3 - see the [LICENSE.md](LICENSE.md) file for details


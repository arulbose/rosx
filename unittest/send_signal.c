
/* Send timer ticks if the kernel is built for x86 simulator */
#include  <stdio.h>
#include  <sys/types.h>
#include  <signal.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <time.h>

struct shm_struct {
        pid_t pid;
	int num;
};

void  main(void)
{
     pid_t   pid;
     key_t MyKey;
     int a;
     int   ShmID;
     struct shm_struct *shm_ptr;
     char  line[100], c;
     int   i;
     int counter = 0;
      struct timespec tim, tim2;
     tim.tv_sec = 0;
     tim.tv_nsec = 100000000L;

     /* generates an IPC key */
     MyKey   = ftok("../", 'c');
     /* Allocates a System V shared memory segment */        
     ShmID   = shmget(MyKey, sizeof(struct shm_struct), 0666);
     /* attaches the System V shared memory segment identified by the ShmID */
     shm_ptr  = (struct shm_struct *) shmat(ShmID, NULL, 0);
     pid     = shm_ptr->pid;                
     //shmdt(shm_ptr);                    

     while (1) {                       
     //     printf("interrupt ?:");
//	  scanf("%d", &a);
	  shm_ptr->num = 0;
          kill(pid, SIGUSR1);
#if 1
	  counter ++;
	  printf("interrupt sent %d\n", counter);
	  nanosleep(&tim , &tim2);
#endif
          }
}

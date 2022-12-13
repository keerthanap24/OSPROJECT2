#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
sem_t *mutex;

void  ChildProcess(int *SharedMemory) {
  int i, accountID, random_num;
  srand(getpid());
  
  for(i=0; i<25;i++) {
    sleep(rand() % 6);
    sem_wait(mutex);
    accountID = *SharedMemory;
    random_num = rand() % 51;
    printf("Poor Student needs $%d\n", random_num);
    
    if (random_num <= accountID) {
      accountID -= random_num;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", random_num, accountID);
      *SharedMemory = accountID;     
    }  
    else {
      printf("Poor Student: Not Enough Cash ($%d)\n", accountID );
    }
    sem_post(mutex);
  }
}

void ParentProcess(int *SharedMemory) {
  int i, accountID, random_num;
  srand(getpid()); // seeds pseudo random number generator
  
  for(i=0; i<25;i++){
    sleep(rand() % 6);
    sem_wait(mutex);
    accountID = *SharedMemory;  
    if (accountID <= 100) { // deposits the money
      random_num = rand() % 101; // number between 0-100
      if (random_num % 2 == 0) { 
        accountID += random_num;     
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", random_num, accountID);    
      }
      else {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
      *SharedMemory = accountID; 
    } 
    else  {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", accountID);
    }
    sem_post(mutex);
  }
}

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

      /* create, initialize semaphore */
     if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) 
     {
          perror("semaphore initilization");
          exit(1);
      }

      // one 
     ShmID = shmget(IPC_PRIVATE, 1*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");
  
     *ShmPTR = 0; // bank accountID taking the contents from the pointer 
    //  ShmPTR[1] = 0; // turn
     //printf("Main has filled bank accountID = %d turn = %d in shared memory...\n",
     //      ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

     printf("Main is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (Main) ***\n");
          exit(1);
     }
     else if (pid == 0) { // poor student
          ChildProcess(ShmPTR);
     }
     else {
          ParentProcess(ShmPTR);
     }
     wait(&status);
     printf("Main has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Main has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Main has removed its shared memory...\n");
     printf("Main exits...\n");
     exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SEMKEY 24604
#define SHMKEY 24603
#define SEG_SIZE 200

union semun {
  int val;               /* Value for SETVAL */
  struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

int main()
{
  printf("trying to get in \n");

  int semd, shmd, fd;
  int *size;
  union semun us;
  struct sembuf sb;

  semd = semget(SEMKEY, 1, 0);
  if (semd == -1)
  {
    printf("check if a story was created; error %d @ semd: %s\n", errno, strerror(errno));
  }

  while (semctl(semd, 0, GETVAL, us) == 0)
    ;

  shmd = shmget(SHMKEY, 0, 0);
  if (shmd == -1)
  {
    printf("check if a story was created; error %d @ shmd: %s\n", errno, strerror(errno));
  }
  size = shmat(shmd, 0, 0);

  sb.sem_num = 0;
  sb.sem_op = -1;
  semop(semd, &sb, 1);

  char update[*size];
  fd = open("story.txt", O_RDONLY);
  if (fd == -1)
  {
    printf("error %d @ fd: %s\n", errno, strerror(errno));
  }
  lseek(fd, -1 * *size, SEEK_END);

  read(fd, update, *size);
  update[*size] = '\0';

  printf("Last addition: %s \n", update);
  printf("Your addition: ");

  char added[200];
  fgets(added, sizeof(added), stdin);
  int len = strlen(added);

  fd = open("story.txt", O_WRONLY | O_APPEND);
  if (fd == -1)
  {
    printf("error %d @ fd: %s\n", errno, strerror(errno));
  }

  write(fd, added, len);
  *size = strlen(added);
  printf("The story file has now been updated. \n");

  shmdt(size);
  sb.sem_op = 1;
  semop(semd, &sb, 1);

  return 0;
}
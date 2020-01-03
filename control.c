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
#define SEG_SIZE 100

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
};

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        int semd, shmd, fd;
        union semun us;
        if (!strcmp(argv[1], "-c"))
        {
            semd = semget(SEMKEY, 1, IPC_CREAT | IPC_EXCL | 0644);
            if (semd == -1)
            {
                printf("check if story exists already; error %d @ semd: %s\n", errno, strerror(errno));
            }

            us.val = 1;
            semctl(semd, 0, SETVAL, us);
            printf("semaphore created \n");

            shmd = shmget(SHMKEY, sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
            if (shmd == -1)
            {
                printf("check if story exists already; error %d @ shmd: %s\n", errno, strerror(errno));
            }
            printf("shared memory created \n");

            fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                printf("error %d @ fd: %s\n", errno, strerror(errno));
            }
            printf("file created \n");

            return 0;
        }
        else if (!strcmp(argv[1], "-v"))
        {
            fd = open("story.txt", O_RDONLY);
            if (fd == -1)
            {
                printf("check if you have a story file; error %d @ fd: %s\n", errno, strerror(errno));
            }
            char content[SEG_SIZE];
            content[0] = '\0';
            read(fd, content, SEG_SIZE);

            if (strlen(content) != 0)
            {
                *(strrchr(content, '\n') + 1) = '\0';
            }

            printf("The story so far: \n");
            printf("%s", content);
            close(fd);

            return 0;
        }
        else if (!strcmp(argv[1], "-r"))
        {
            printf("trying to get in \n");

            semd = semget(SEMKEY, 1, 0);
            if (semd == -1)
            {
                printf("error %d @ semd: %s\n", errno, strerror(errno));
            }

            while (!semctl(semd, 0, GETVAL, us))
                ;

            fd = open("story.txt", O_RDONLY);
            if (fd == -1)
            {
                printf("check if you have a story file; error %d @ fd: %s\n", errno, strerror(errno));
            }
            char content[SEG_SIZE];
            content[0] = '\0';
            read(fd, content, SEG_SIZE);

            if (strlen(content) != 0)
                *(strrchr(content, '\n') + 1) = '\0';

            printf("The story so far: \n");
            printf("%s \n", content);
            close(fd);

            shmd = shmget(SHMKEY, 0, 0);
            if (shmd == -1)
            {
                printf("error %d @ shmd: %s\n", errno, strerror(errno));
            }

            shmctl(shmd, IPC_RMID, 0);
            printf("shared memory removed \n");

            remove("story.txt");
            printf("file removed \n");

            semctl(semd, IPC_RMID, 0);
            printf("semaphore removed \n");

            return 0;
        }
        else
        {
            printf("Please input -c, -v, or -r as an argument to this program.\n");
            return 1;
        }
    }
    else if (argc)
    {
        printf("Please input -c, -v, or -r as an argument to this program.\n");
        return 1;
    }
    printf("Please provide the correct input to this program.\n");
    return 1;
}
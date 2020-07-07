/*
https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c
https://www.youtube.com/watch?v=SMeDw2GDMsE
https://man7.org/linux/man-pages/man2/shmget.2.html
https://man7.org/linux/man-pages/man3/ftok.3.html
https://man7.org/linux/man-pages/man3/ftok.3.html
https://stackoverflow.com/questions/7358400/whats-is-purpose-ftok-in-message-queues
https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_73/apis/ipcshmgt.htm
https://stackoverflow.com/questions/35992378/the-need-for-a-key-in-system-v-ipc
https://stackoverflow.com/questions/7358400/whats-is-purpose-ftok-in-message-queues
https://stackoverflow.com/questions/2068498/how-to-choose-the-key-for-inter-processes-communication-in-linux
https://www.youtube.com/watch?v=ZuJCPjN6rU8
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

int main(int argc, char *argv[])
{
    key_t key;
    int shmid;
    char *data;
    int mode;

    if (argc > 2) {
        fprintf(stderr, "usage: shmdemo [data_to_write]\n");
        exit(1);
    }

    /* make the key: */
    if ((key = ftok("hello.txt", 'R')) == -1) /*Here the file must exist */ 
{
        perror("ftok");
        exit(1);
    }

    /*  create the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, NULL, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    /* read or modify the segment, based on the command line: */
    if (argc == 2) {
        printf("writing to segment: \"%s\"\n", argv[1]);
        strncpy(data, argv[1], SHM_SIZE);
    } else
        printf("segment contains: \"%s\"\n", data);

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}

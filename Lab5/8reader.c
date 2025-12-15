#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "shm.h"

int main(void)
{
    int semid, fd, transfers, bytes;
    struct shmseg *shmp;

    /* Get semaphore set */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    /* Open backing file */
    fd = open(SHM_FILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* Map shared memory */
    shmp = mmap(NULL, sizeof(struct shmseg),
                PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);

    if (shmp == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    /* Read loop */
    for (transfers = 0, bytes = 0;; transfers++) {
        reserveSem(semid, READ_SEM);

        if (shmp->cnt == 0)
            break;

        bytes += shmp->cnt;
        write(STDOUT_FILENO, shmp->buf, shmp->cnt);

        releaseSem(semid, WRITE_SEM);
    }

    munmap(shmp, sizeof(struct shmseg));
    releaseSem(semid, WRITE_SEM);

    fprintf(stderr, "Received %d bytes (%d transfers)\n", bytes, transfers);
    return 0;
}

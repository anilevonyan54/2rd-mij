#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "semun.h"
#include "shm.h"

int main(void)
{
    int semid, fd, bytes, transfers;
    struct shmseg *shmp;
    union semun dummy;

    /* Create semaphore set */
    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    initSemAvailable(semid, WRITE_SEM);
    initSemInUse(semid, READ_SEM);

    /* Create backing file for mmap */
    fd = open(SHM_FILE, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* Resize file */
    ftruncate(fd, sizeof(struct shmseg));

    /* Map shared memory */
    shmp = mmap(NULL, sizeof(struct shmseg),
                PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);

    if (shmp == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    /* Write loop */
    for (transfers = 0, bytes = 0;; transfers++, bytes += shmp->cnt) {
        reserveSem(semid, WRITE_SEM);

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        releaseSem(semid, READ_SEM);

        if (shmp->cnt == 0)
            break;
    }

    reserveSem(semid, WRITE_SEM);

    munmap(shmp, sizeof(struct shmseg));
    unlink(SHM_FILE);
    semctl(semid, 0, IPC_RMID, dummy);

    printf("Sent %d bytes (%d transfers)\n", bytes, transfers);
    return 0;
}

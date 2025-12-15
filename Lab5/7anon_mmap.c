#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main(void)
{
    int *addr;

    /* Anonymous PRIVATE mapping (Linux-supported) */
    addr = mmap(NULL,
                sizeof(int),
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    *addr = 1;   /* Initialize value */

    switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0: /* Child */
            printf("Child started, value = %d\n", *addr);
            (*addr)++;   /* Triggers copy-on-write */
            munmap(addr, sizeof(int));
            exit(EXIT_SUCCESS);

        default: /* Parent */
            wait(NULL);
            printf("In parent, value = %d\n", *addr);
            munmap(addr, sizeof(int));
            exit(EXIT_SUCCESS);
    }
}

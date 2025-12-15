#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MEM_SIZE 15

int main(int argc, char *argv[])
{
    char *addr;
    int fd;

    if (argc < 2) {
        fprintf(stderr, "Usage error: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, MEM_SIZE,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    /* First read — valid */
    printf("Current string = %.*s\n", MEM_SIZE, addr);

    if (argc > 2) {
        if (strlen(argv[2]) >= MEM_SIZE) {
            fprintf(stderr, "'new-value' too large\n");
            exit(EXIT_FAILURE);
        }

        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);

        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            perror("msync");
            exit(EXIT_FAILURE);
        }

        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }

    /* ✅ UNMAP the region */
    if (munmap(addr, MEM_SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    printf("\nMapping removed with munmap()\n");

    /* ❌ INVALID ACCESS — reading after unmap */
    printf("Attempting to read after munmap...\n");
    printf("String = %.*s\n", MEM_SIZE, addr);  /* UNDEFINED BEHAVIOR */

    return 0;
}

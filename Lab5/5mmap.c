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
        fprintf(stderr, "Usage: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* ✅ MAP_PRIVATE instead of MAP_SHARED */
    addr = mmap(NULL, MEM_SIZE,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE,     /* <-- key change */
                fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    printf("Current string = %.*s\n", MEM_SIZE, addr);

    if (argc > 2) {
        if (strlen(argv[2]) >= MEM_SIZE) {
            fprintf(stderr, "'new-value' too large\n");
            exit(EXIT_FAILURE);
        }

        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);

        /* This msync will NOT update the file */
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            perror("msync");
            exit(EXIT_FAILURE);
        }

        printf("Copied \"%s\" to PRIVATE mapping\n", argv[2]);
    }

    munmap(addr, MEM_SIZE);
    return 0;
}

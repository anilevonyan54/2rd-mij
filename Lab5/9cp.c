#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
    int fd_src, fd_dst;
    ssize_t num_read;
    char buf[BUF_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src> <dest>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Open source file (must exist) */
    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    /* Open destination file, create if needed */
    fd_dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_dst == -1) {
        perror("Error opening/creating destination file");
        close(fd_src);
        exit(EXIT_FAILURE);
    }

    /* Copy loop */
    while ((num_read = read(fd_src, buf, BUF_SIZE)) > 0) {
        if (write(fd_dst, buf, num_read) != num_read) {
            perror("Write error");
            close(fd_src);
            close(fd_dst);
            exit(EXIT_FAILURE);
        }
    }

    if (num_read == -1) {
        perror("Read error");
    }

    close(fd_src);
    close(fd_dst);

    return 0;
}

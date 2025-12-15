#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "unix_stream.h"

int main(void)
{
    struct sockaddr_un addr;
    int sfd;
    char buf[BUF_SIZE];
    ssize_t n;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *)&addr,
                sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* stdin → socket */
    while ((n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {

        if (write(sfd, buf, n) != n) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        /* socket → stdout */
        n = read(sfd, buf, BUF_SIZE);
        if (n <= 0)
            break;

        write(STDOUT_FILENO, buf, n);
    }

    close(sfd);
    return 0;
}

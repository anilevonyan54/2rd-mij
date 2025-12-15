#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "unix_stream.h"

#define BACKLOG 5

int main(void)
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) { perror("socket"); exit(1); }

    unlink(SV_SOCK_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    for (;;) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) { perror("accept"); continue; }

        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {

            /* THE ECHO LINE — MOST IMPORTANT */
            if (write(cfd, buf, numRead) != numRead) {
                perror("write");
                exit(1);
            }
        }

        close(cfd);
    }
}

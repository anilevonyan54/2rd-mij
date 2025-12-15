#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include "unix_stream.h"

int main(void)
{
    int sfd;
    struct sockaddr_un addr;
    fd_set rset;
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

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to Chatroom Armenia 🇦🇲\n");

    for (;;) {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sfd, &rset);

        if (select(sfd + 1, &rset, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sfd, &rset)) {
            n = read(sfd, buf, BUF_SIZE);
            if (n <= 0) {
                printf("Server disconnected\n");
                break;
            }
            write(STDOUT_FILENO, buf, n);
        }

        if (FD_ISSET(STDIN_FILENO, &rset)) {
            n = read(STDIN_FILENO, buf, BUF_SIZE);
            if (n > 0)
                write(sfd, buf, n);
        }
    }

    close(sfd);
    return 0;
}

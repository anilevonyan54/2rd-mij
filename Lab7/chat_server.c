#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include "unix_stream.h"

#define BACKLOG 10
#define MAX_CLIENTS FD_SETSIZE

int main(void)
{
    int sfd, cfd, maxfd;
    int clients[MAX_CLIENTS];
    struct sockaddr_un addr;
    fd_set allset, rset;
    char buf[BUF_SIZE];
    ssize_t n;

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SV_SOCK_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("🇦🇲 Chatroom Armenia server started\n");

    FD_ZERO(&allset);
    FD_SET(sfd, &allset);
    maxfd = sfd;

    for (;;) {
        rset = allset;

        if (select(maxfd + 1, &rset, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sfd, &rset)) {
            cfd = accept(sfd, NULL, NULL);
            if (cfd == -1)
                continue;

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] < 0) {
                    clients[i] = cfd;
                    break;
                }
            }

            FD_SET(cfd, &allset);
            if (cfd > maxfd)
                maxfd = cfd;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = clients[i];
            if (sock < 0)
                continue;

            if (FD_ISSET(sock, &rset)) {
                n = read(sock, buf, BUF_SIZE);
                if (n <= 0) {
                    close(sock);
                    FD_CLR(sock, &allset);
                    clients[i] = -1;
                } else {
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clients[j] >= 0 && clients[j] != sock) {
                            write(clients[j], buf, n);
                        }
                    }
                }
            }
        }
    }
}

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 50000
#define MAX_CLIENTS  FD_SETSIZE
#define BUF_SIZE 1024

void errExit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int listenfd, maxfd, clientfd;
    int clients[MAX_CLIENTS];
    struct sockaddr_in addr;
    fd_set allset, rset;
    char buf[BUF_SIZE];
    ssize_t n;

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        errExit("socket");

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        errExit("bind");

    if (listen(listenfd, 10) == -1)
        errExit("listen");

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;

    printf("Chat server running on port %d\n", PORT);

    for (;;) {
        rset = allset;

        if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0)
            errExit("select");

        /* New client */
        if (FD_ISSET(listenfd, &rset)) {
            clientfd = accept(listenfd, NULL, NULL);
            if (clientfd == -1)
                continue;

            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] < 0) {
                    clients[i] = clientfd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                close(clientfd); // too many clients
            } else {
                FD_SET(clientfd, &allset);
                if (clientfd > maxfd)
                    maxfd = clientfd;

                printf("Client connected (fd=%d)\n", clientfd);
            }
        }

        /* Existing clients */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = clients[i];
            if (sock < 0) continue;

            if (FD_ISSET(sock, &rset)) {
                n = read(sock, buf, BUF_SIZE);
                if (n <= 0) {
                    printf("Client disconnected (fd=%d)\n", sock);
                    close(sock);
                    FD_CLR(sock, &allset);
                    clients[i] = -1;
                } else {
                    /* Broadcast to others */
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int other = clients[j];
                        if (other >= 0 && other != sock) {
                            write(other, buf, n);
                        }
                    }
                }
            }
        }
    }
}

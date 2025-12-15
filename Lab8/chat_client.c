#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 50000
#define BUF_SIZE 1024

void errExit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;
    fd_set rset;
    char buf[BUF_SIZE];
    ssize_t n;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        errExit("socket");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        errExit("inet_pton");

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        errExit("connect");

    printf("Connected to chat server\n");

    for (;;) {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sockfd, &rset);

        if (select(sockfd + 1, &rset, NULL, NULL, NULL) < 0)
            errExit("select");

        /* Message from server */
        if (FD_ISSET(sockfd, &rset)) {
            n = read(sockfd, buf, BUF_SIZE);
            if (n <= 0) {
                printf("Server closed connection\n");
                break;
            }
            write(STDOUT_FILENO, buf, n);
        }

        /* Input from user */
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            n = read(STDIN_FILENO, buf, BUF_SIZE);
            if (n > 0) {
                write(sockfd, buf, n);
            }
        }
    }

    close(sockfd);
    return 0;
}

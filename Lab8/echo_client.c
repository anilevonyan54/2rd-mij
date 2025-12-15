#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define PORT 50000

void errExit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_in svaddr;
    char buf[BUF_SIZE];
    ssize_t numRead;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        errExit("socket");

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
        errExit("inet_pton");

    if (connect(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1)
        errExit("connect");

    fprintf(stderr, "Connected. Type and press ENTER.\n");

    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {

        ssize_t totalSent = 0;
        while (totalSent < numRead) {
            ssize_t n = write(sfd, buf + totalSent, numRead - totalSent);
            if (n <= 0)
                errExit("write");
            totalSent += n;
        }

        numRead = read(sfd, buf, BUF_SIZE);
        if (numRead <= 0)
            break;

        write(STDOUT_FILENO, buf, numRead);
        fsync(STDOUT_FILENO);
    }

    close(sfd);
    return 0;
}

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 50000
#define BUF_SIZE 1024

void errExit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sfd, cfd;
    struct sockaddr_in svaddr, claddr;
    socklen_t cllen;
    char buf[BUF_SIZE];
    ssize_t numRead;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        errExit("socket");

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = INADDR_ANY;
    svaddr.sin_port = htons(PORT);

    if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1)
        errExit("bind");

    if (listen(sfd, 10) == -1)
        errExit("listen");

    printf("Echo server listening on port %d...\n", PORT);

    for (;;) {
        cllen = sizeof(claddr);
        cfd = accept(sfd, (struct sockaddr *)&claddr, &cllen);
        if (cfd == -1)
            continue;

        if (fork() == 0) {  // child
            close(sfd);

            while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
                write(cfd, buf, numRead); // echo back
            }

            close(cfd);
            _exit(EXIT_SUCCESS);
        }

        close(cfd); // parent
    }
}

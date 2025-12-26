#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "protocol.h"

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }


    char buffer[512];

    fd_set rfds;
    struct timeval tv;

    while (1) {
      printf("cmd> ");
      fflush(stdout);

    if (!fgets(buffer, sizeof(buffer), stdin))
        break;

    send(sock, buffer, strlen(buffer), 0);

    /* Read all available server output */
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 300000; // 300 ms

        int ret = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (ret <= 0)
            break;

        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0)
            break;

        buffer[n] = '\0';
        printf("%s", buffer);
    }
}

    close(sock);
    return 0;
}



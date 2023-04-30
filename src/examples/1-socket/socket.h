#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

typedef struct sockaddr_in SockaddrIn;
typedef struct sockaddr Sockaddr;

typedef enum {
    SOCKET_SERVER,
    SOCKET_CLIENT,
} SocketType;

int socket_init(SockaddrIn* s, char* server_ip, int port, SocketType type) {
    int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        perror("Not able to open socket");
        return -1;
    }

    memset(s, 0, sizeof(SockaddrIn));
    s->sin_family = AF_INET;
    s->sin_port = htons(port);

    if (type == SOCKET_SERVER) {
        s->sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(fd, (Sockaddr*)s, sizeof(SockaddrIn)) < 0) {
            perror("Not able to bind socket");
            return -1;
        }
    } else if (type == SOCKET_CLIENT) {
        s->sin_addr.s_addr = inet_addr(server_ip);

        if (connect(fd, (Sockaddr*)s, sizeof(SockaddrIn)) < 0) {
            perror("Failed to connect to server");
            return -1;
        }
    } else {
        fprintf(stderr, "Not recognized socket type %d\n", type);
        return -1;
    }

    return fd;
}

#endif // SOCKET_H_INCLUDED
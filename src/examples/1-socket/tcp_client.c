#include "defs.h"
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <server port>\n", argv[0]);
        exit(-1);
    }

    int client_fd;
    char* server_ip = argv[1];
    u16 server_port = atoi(argv[2]);

    SockaddrIn client;
    SockaddrIn server;
    client_fd = socket_init(&server, server_ip, server_port, SOCKET_CLIENT);
    if (client_fd < 0) {
        perror("Failed to init client");
        exit(1);
    }

    unsigned int client_size = sizeof(client);
    getsockname(client_fd, (Sockaddr*)&client, &client_size);

    printf("Client bound to port %d\n", client.sin_port);

    const char* msg = "Hello from client!\n";
    int msg_len = strlen(msg);
    if (send(client_fd, msg, msg_len, 0) != msg_len)
        perror("Failed to send message.");

    char receive_buff[32];

    recv(client_fd, receive_buff, 31, 0);
    send(client_fd, receive_buff, 31, 0);

    printf("Received: [%s]\n", receive_buff);

    close(client_fd);
    return 0;
}
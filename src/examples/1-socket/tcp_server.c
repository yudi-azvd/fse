#include "../common/socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_client(int client_fd) {
    char buffer[16];
    int tamanhoRecebido;

    if ((tamanhoRecebido = recv(client_fd, buffer, 16, 0)) < 0)
        printf("1 Erro no recv()\n");

    while (tamanhoRecebido > 0) {
        if (send(client_fd, buffer, tamanhoRecebido, 0) != tamanhoRecebido)
            printf("Erro no envio - send()\n");

        if ((tamanhoRecebido = recv(client_fd, buffer, 16, 0)) < 0) {
            printf("Erro no recv()\n");

        } else
            printf("Server recv: %s\n", buffer);
    }
}

int main(int argc, char** argv) {
    int server_fd;
    int client_fd;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(-1);
    }

    int server_port = atoi(argv[1]);

    SockaddrIn client;
    SockaddrIn server;
    server_fd = socket_init(&server, NULL, server_port, SOCKET_SERVER);
    if (server_fd < 0) {
        perror("Failed to init server");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Failed to listen");
        exit(2);
    }

    while (1) {
        uint32_t client_len = sizeof(client);
        client_fd = accept(server_fd, (Sockaddr*)&client, &client_len);
        if (client_fd < 0)
            perror("Failed accept");

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, str, INET_ADDRSTRLEN);
        printf("CLIENT INFO %s:%d\n", str, client.sin_port);
        handle_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
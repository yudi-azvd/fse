#ifndef UART_PROTO_H_INCLUDED
#define UART_PROTO_H_INCLUDED

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "mssleep.h"

typedef uint8_t u8;

#define READ_INT 0xA1
#define READ_FLOAT 0xA2
#define READ_STR 0xA3

#define WRITE_INT 0xB1
#define WRITE_FLOAT 0xB2
#define WRITE_STR 0xB3

#define UART_PROTO "UART proto: "
#define UART_PROTO_ERROR_SIZE 256
char uart_proto_error[UART_PROTO_ERROR_SIZE];

void uart_proto_perror(char* msg = NULL) {
    if (msg)
        printf("%s\n", uart_proto_error);
    else
        printf("%s%s\n", msg, uart_proto_error);
}

typedef struct _Base {
    int uart_fd;
    termios options;
} _Base;

#define init_signature(p)   \
    {                       \
        p.signature[0] = 9; \
        p.signature[1] = 4; \
        p.signature[2] = 1; \
        p.signature[3] = 0; \
    }

#pragma pack(1)
typedef struct RequestPacket {
    u8 code;
    u8 signature[4];
} RequestPacket;

RequestPacket RequestPacket_init(u8 code) {
    RequestPacket p;
    init_signature(p);
    p.code = code;
    return p;
}

#pragma pack(1)
typedef struct WriteIntPacket {
    u8 code;
    int data;
    u8 signature[4];
} WriteIntPacket;

WriteIntPacket WriteIntPacket_init(int data) {
    WriteIntPacket p;
    init_signature(p);
    p.code = WRITE_INT;
    p.data = data;
    return p;
}

#pragma pack(1)
typedef struct WriteFloatPacket {
    u8 code;
    float data;
    u8 signature[4];
} WriteFloatPacket;

WriteFloatPacket WriteFloatPacket_init(float data) {
    WriteFloatPacket p;
    init_signature(p);
    p.code = WRITE_FLOAT;
    p.data = data;
    return p;
}

#define PACKET_STR_CAPACITY 256
#pragma pack(1)
typedef struct WriteStrPacket {
    u8 code;
    u8 size;
    char data[PACKET_STR_CAPACITY - 1];
    u8 signature[4];
} WriteStrPacket;

WriteStrPacket WriteStrPacket_init(char* data) {
    WriteStrPacket p;
    init_signature(p);
    p.code = WRITE_STR;
    p.size = strlen(data);
    strncpy(p.data, data, PACKET_STR_CAPACITY - 1);
    return p;
}

int _uart_proto_init(_Base* b, char* device = NULL) {
    b->uart_fd = -1;
    if (device)
        b->uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    else
        b->uart_fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (b->uart_fd == -1) {
        perror("not able to init UART");
        strncpy(uart_proto_error, UART_PROTO "not able to init UART", UART_PROTO_ERROR_SIZE);
        strncat(uart_proto_error, strerror(errno), 9);
        return b->uart_fd;
    }

    tcgetattr(b->uart_fd, &b->options);
    b->options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    b->options.c_iflag = IGNPAR;
    b->options.c_oflag = 0;
    b->options.c_lflag = 0;
    tcflush(b->uart_fd, TCIFLUSH);
    tcsetattr(b->uart_fd, TCSANOW, &b->options);

    return b->uart_fd;
}

int _uart_proto_destroy(_Base* b) {
    close(b->uart_fd);
    return 0;
}

int uart_proto_read_int(int* result) {
    _Base b;
    _uart_proto_init(&b);

    RequestPacket p = RequestPacket_init(READ_INT);
    ssize_t packet_size = sizeof(RequestPacket);
    ssize_t written = write(b.uart_fd, &p, packet_size);
    if (written < 0) {
        perror("UART proto write int");
    }

    msleep(100);
    int data;
    ssize_t read_size = read(b.uart_fd, &data, sizeof(data));
    if (read_size < 0) {
        perror("UART proto read int");
    }
    *result = data;

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_read_float(float* result) {
    _Base b;
    _uart_proto_init(&b);

    RequestPacket p = RequestPacket_init(READ_FLOAT);
    ssize_t packet_size = sizeof(RequestPacket);
    ssize_t written = write(b.uart_fd, &p, packet_size);
    if (written < 0) {
        perror("UART proto write");
    }

    msleep(100);
    float data;
    ssize_t read_size = read(b.uart_fd, &data, sizeof(data));
    if (read_size < 0) {
        perror("UART proto read float");
    }
    *result = data;

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_read_str(char** result) {
    _Base b;
    _uart_proto_init(&b);

    RequestPacket p = RequestPacket_init(READ_STR);
    ssize_t packet_size = sizeof(RequestPacket);
    ssize_t written = write(b.uart_fd, &p, packet_size);
    if (written < 0) {
        perror("UART proto write");
    }
    msleep(100);

    size_t str_size = 0;
    ssize_t read_size = read(b.uart_fd, &str_size, sizeof(char));
    if (read_size < 0) {
        perror("UART proto read string size");
    }

    char data[256];
    read_size = read(b.uart_fd, data, str_size);
    if (read_size < 0) {
        perror("UART proto read string data");
    }
    // Código | tamanho str | conteúdo str | signature
    *result = data;

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_write_int(int data) {
    _Base b;
    _uart_proto_init(&b);

    WriteIntPacket p = WriteIntPacket_init(data);
    ssize_t packet_size = sizeof(p);
    ssize_t written = write(b.uart_fd, &p, packet_size);
    if (written < 0) {
        perror("UART proto write int");
    }

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_write_float(float data) {
    _Base b;
    _uart_proto_init(&b);

    WriteFloatPacket p = WriteFloatPacket_init(data);
    ssize_t packet_size = sizeof(p);
    ssize_t written = write(b.uart_fd, &p, packet_size);
    if (written < 0) {
        perror("UART proto write float");
    }

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_write_string(char* data) {
    _Base b;
    _uart_proto_init(&b);

    WriteStrPacket p = WriteStrPacket_init(data);
    ssize_t written = write(b.uart_fd, &p.code, sizeof(u8));
    if (written < 0) {
        perror("UART proto write code");
    }

    u8 data_size = strlen(data);
    written = write(b.uart_fd, &data_size, sizeof(data_size));
    if (written < 0) {
        perror("UART proto write data size");
    }

    written = write(b.uart_fd, data, data_size);
    if (written < 0) {
        perror("UART proto write string");
    }

    written = write(b.uart_fd, p.signature, sizeof(p.signature));
    if (written < 0) {
        perror("UART proto write signature");
    }

    _uart_proto_destroy(&b);
    return 0;
}
// b o m b a
// 1 6 4

#endif // UART_PROTO_H_INCLUDED
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
    int uart0_filestream;
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

WriteIntPacket WriteIntPacket_init(u8 code, int data) {
    WriteIntPacket p;
    init_signature(p);
    p.code = code;
    p.data = data;
    return p;
}

int _uart_proto_init(_Base* b, char* device = NULL) {
    b->uart0_filestream = -1;
    if (device)
        b->uart0_filestream = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    else
        b->uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (b->uart0_filestream == -1) {
        perror("not able to init UART");
        strncpy(uart_proto_error, UART_PROTO "not able to init UART", UART_PROTO_ERROR_SIZE);
        strncat(uart_proto_error, strerror(errno), 9);
        return b->uart0_filestream;
    }

    tcgetattr(b->uart0_filestream, &b->options);
    b->options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    b->options.c_iflag = IGNPAR;
    b->options.c_oflag = 0;
    b->options.c_lflag = 0;
    tcflush(b->uart0_filestream, TCIFLUSH);
    tcsetattr(b->uart0_filestream, TCSANOW, &b->options);

    return b->uart0_filestream;
}

int _uart_proto_destroy(_Base* b) {
    close(b->uart0_filestream);
    return 0;
}

int uart_proto_read_int(int* result) {
    _Base b;
    _uart_proto_init(&b);

    RequestPacket p = RequestPacket_init(READ_INT);
    ssize_t packet_size = sizeof(RequestPacket);
    ssize_t written = write(b.uart0_filestream, &p, packet_size);
    if (written < 0) {
        perror("UART proto write");
    }

    int data;
    ssize_t read_size = read(b.uart0_filestream, &data, 4);
    if (read_size < 0) {
        perror("UART proto read");
    }
    *result = data;

    _uart_proto_destroy(&b);
    return 0;
}

int uart_proto_write_int(int data) {
    _Base b;
    _uart_proto_init(&b);

    WriteIntPacket p;
    p.data = data;
    p.code = WRITE_INT;
    ssize_t packet_size = sizeof(p);
    ssize_t written = write(b.uart0_filestream, &p, packet_size);
    if (written < 0) {
    }

    ssize_t read_size = read(b.uart0_filestream, &data, 4);
    if (read_size < 0) {
    }

    _uart_proto_destroy(&b);
    return 0;
}

#endif // UART_PROTO_H_INCLUDED
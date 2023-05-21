#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "defs.h"

#define CODE_READ 0x23
#define CODE_WRITE 0x16
#define SUBCODE_READ_INT 0xA1
#define SUBCODE_READ_FLOAT 0xA2
#define SUBCODE_READ_STR 0xA3
#define SUBCODE_WRITE_INT 0xB1
#define SUBCODE_WRITE_FLOAT 0xB2
#define SUBCODE_WRITE_STR 0xB3

#define CHECK_FD(fd)                   \
    if (fd < 0) {                      \
        perror("Failed to open UART"); \
        return fd;                     \
    }

#pragma pack(1)
typedef struct {
    u8 code;
    u8 subcode;
    u8 command;
} BasePacket;

#define USE_COMMON_PACKET_MEMBERS \
    u16 crc16;                    \
    u8 address;                   \
    u8 code;                      \
    u8 subcode;                   \
    u8 command;

typedef struct
{
    union data_t {
        f32 f;
        i32 i;
    };
    USE_COMMON_PACKET_MEMBERS;
    data_t data;
} NumberPacket;

typedef struct
{
    USE_COMMON_PACKET_MEMBERS;
    u8 size;
    char str[256];
} StrPacket;

int _modbus_open(char* path = "/dev/serial0") {
    int uart_fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        return uart_fd;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    return uart_fd;
}

int modbus_read_int(int* result) {
    int fd = _modbus_open();
    CHECK_FD(fd);

    NumberPacket p;
    p.address = 0x01;
    p.code = CODE_READ;
    p.subcode = SUBCODE_READ_INT;

    close(fd);
}

int modbus_close(int fd) {
    return close(fd);
}

#endif // MODBUS_H_INCLUDED
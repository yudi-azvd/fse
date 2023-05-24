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

#include "crc16.h"
#include "defs.h"
#include "mssleep.h"
#include "util.h"

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

#define USE_COMMON_PACKET_MEMBERS \
    u8 address;                   \
    u8 code;                      \
    u8 subcode;

#pragma pack(1)
typedef struct {
    USE_COMMON_PACKET_MEMBERS;
    u16 crc16;
} RequestNumberPacket;

#pragma pack(1)
typedef struct {
    union data_t {
        f32 f;
        i32 i;
    };
    USE_COMMON_PACKET_MEMBERS;
    data_t data;
    u16 crc16;
} NumberPacket;

// typedef struct {
//     USE_COMMON_PACKET_MEMBERS;
//     u8 size;
//     char str[256];
//     u16 crc16;
// } StrPacket;

i32 _modbus_open(const char* path = "/dev/serial0") {
    int uart_fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
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

i32 _modbus_close(int fd) {
    return close(fd);
}

i32 modbus_read_int(i32* result) {
    int return_value = 0;
    int fd = _modbus_open();
    CHECK_FD(fd);

    RequestNumberPacket request_packet;
    i32 packet_size = sizeof(request_packet);
    u8* packet_ptr = (u8*)&request_packet;
    request_packet.address = 0x01;
    request_packet.code = CODE_READ;
    request_packet.subcode = SUBCODE_READ_INT;
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);

    request_packet.crc16 = calculate_crc((u8*)&request_packet, packet_size - sizeof(request_packet.crc16));
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);
    printf("         crc16 0x%04X\n", request_packet.crc16);

    int write_size = write(fd, &request_packet, packet_size);
    if (write_size <= 0) {
        perror("Failed at writing int");
    }

    msleep(100);

    NumberPacket np;
    i32 read_size = read(fd, &np, sizeof(np));
    if (write_size <= 0) {
        perror("Failed at reading int");
    }

    u16 expected_crc = calculate_crc((u8*)&np, sizeof(np) - sizeof(np.crc16));
    if (expected_crc != np.crc16) {
        perror("Corrupted number packet");
        return_value = -1;
    }

    *result = np.data.i;
    printf("result %d\n", *result);

    _modbus_close(fd);
    return return_value;
}

i32 modbus_read_float(f32* result) {
    i32 return_value = 0;
    i32 fd = _modbus_open();
    CHECK_FD(fd);

    RequestNumberPacket p = {0};
    i32 packet_size = sizeof(p);
    u8* packet_ptr = (u8*)&p;
    p.address = 0x01;
    p.code = CODE_READ;
    p.subcode = SUBCODE_READ_FLOAT;
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);
    p.crc16 = calculate_crc((u8*)&p, packet_size - sizeof(p.crc16));
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);
    printf("         crc16 0x%X\n", p.crc16);

    i32 write_size = write(fd, &p, packet_size);
    if (write_size <= 0) {
        perror("Failed at writing int");
    }

    msleep(100);

    NumberPacket np;
    i32 read_size = read(fd, &np, sizeof(np));
    if (write_size <= 0) {
        perror("Failed at reading int");
    }

    u16 expected_crc = calculate_crc((u8*)&np, sizeof(np) - sizeof(np.crc16));
    if (expected_crc != np.crc16) {
        perror("Corrupted number packet");
        return_value = -1;
    }

    *result = np.data.i;
    printf("result %.2ff\n", *result);

    _modbus_close(fd);
    return return_value;
}

#endif // MODBUS_H_INCLUDED
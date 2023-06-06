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

#pragma pack(1)
typedef struct {
    u8 address;
    u8 code;
    u8 data;
    u16 crc16;
} BasePacket;

#pragma pack(1)
typedef struct {
    u8 address;
    u8 code;
    u8 data[5];
    u16 crc16;
} WriteNumberPacket;

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
    i32 return_value = 0;
    i32 fd = _modbus_open();
    CHECK_FD(fd);

    BasePacket packet;
    i32 packet_size = sizeof(packet);
    u8* packet_ptr = (u8*)&packet;
    packet.address = 0x01;
    packet.code = CODE_READ;
    packet.data = SUBCODE_READ_INT;
    packet.crc16 = calculate_crc((u8*)&packet, packet_size - sizeof(packet.crc16));
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);
    printf("         crc16 0x%04X\n", packet.crc16);

    i32 write_size = write(fd, &packet, packet_size);
    if (write_size <= 0) {
        perror("Failed at writing int");
        return_value = -1;
    }

    // printf("written %d bytes\n", write_size);
    msleep(100);

    WriteNumberPacket p = {0};
    i32 read_size = read(fd, &p, sizeof(p));
    if (write_size <= 0) {
        perror("Failed at reading int");
        return_value = -1;
    }

    u16 expected_crc = calculate_crc((u8*)&p, sizeof(p) - sizeof(p.crc16));
    if (expected_crc != p.crc16) {
        printf(">> expected crc 0x%04X\n", expected_crc);
        printf(">>   result crc 0x%04X\n", p.crc16);
        perror("Corrupted number packet");
        return_value = -1;
    }

    printf("writenumber pck\n");
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);

    // *result = np.data[0];
    memcpy(result, (&p.data) + 1, 4);
    printf("(modbus) result %d\n", *result);

    _modbus_close(fd);
    return return_value;
}

i32 modbus_write_int(i32 data) {
    int return_value = 0;
    int fd = _modbus_open();
    // CHECK_FD(fd);

    WriteNumberPacket p = {0};
    i32 packet_size = sizeof(p);
    u8* packet_ptr = (u8*)&p;

    p.address = 0x01;
    p.code = CODE_WRITE;
    memcpy(&p.data[1], &data, 4);
    p.data[0] = SUBCODE_WRITE_INT;

    packet_ptr = (u8*)&p.data;

    p.crc16 = calculate_crc((u8*)&p, packet_size - sizeof(p.crc16));
    printf("packet content ");
    print_u8_arr(packet_ptr, packet_size);
    printf("         crc16 0x%04X\n", p.crc16);

    packet_ptr = (u8*)&p;

    int write_size = write(fd, &p, packet_size);
    if (write_size <= 0) {
        perror("Failed at writing int");
        return_value = -1;
    }

    return return_value;
}

i32 modbus_write_float(f32 data) {
    int return_value = 0;
    int fd = _modbus_open();
    CHECK_FD(fd);

    WriteNumberPacket p = {0};
    i32 packet_size = sizeof(p);
    u8* packet_ptr = (u8*)&p;

    p.address = 0x01;
    p.code = CODE_WRITE;
    memcpy(&p.data[1], &data, 4);
    p.data[0] = SUBCODE_WRITE_FLOAT;

    packet_ptr = (u8*)&p.data;

    p.crc16 = calculate_crc((u8*)&p, packet_size - sizeof(p.crc16));
    packet_ptr = (u8*)&p;

    int write_size = write(fd, &p, packet_size);
    if (write_size <= 0) {
        perror("Failed at writing float");
        return_value = -1;
    }

    return return_value;
}

#endif // MODBUS_H_INCLUDED
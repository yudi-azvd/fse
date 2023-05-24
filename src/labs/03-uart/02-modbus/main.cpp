#include <stdio.h>

#include "modbus.h"
#include "mssleep.h"

int main() {
    printf("\n");

    i32 i_result = -1;
    if (modbus_read_int(&i_result) < 0) {
        perror("Error modbus_read_int");
    } else {
        printf("read int: %d\n", i_result);
    }

    // f32 f_result = -1;
    // if (modbus_read_float(&f_result) < 0) {
    //     perror("Error modbus_read_float");
    // } else {
    //     printf("read float: %.2f\n", f_result);
    // }

    printf("\n");
    return 0;
}
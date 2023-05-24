#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>

#include "defs.h"

void print_u8_arr(u8* arr, i32 size) {
    printf("0x");
    for (i32 i = 0; i < size; i++)
        printf("%02X", *(arr + i));
    printf("\n");
}

#endif // UTIL_H_INCLUDED
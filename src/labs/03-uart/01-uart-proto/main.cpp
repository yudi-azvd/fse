#include <stdio.h>

#include "uart_proto.h"

int main() {
    Base base;
    if (uart_proto_init(&base) < 0) {
        uart_proto_perror();
        exit(1);
    }

    uart_proto_destroy(&base);
    return 0;
}
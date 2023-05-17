#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#include "kbhit2.h"
#include "uart_proto.h"

#define clear() printf("\033[H\033[J")

enum {
    MENU_READ_INT = 1,
    MENU_READ_FLOAT,
    MENU_READ_STR,
    MENU_WRITE_INT,
    MENU_WRITE_FLOAT,
    MENU_WRITE_STR,
    MENU_EXIT,
};

void print_ui() {
    clear();
    printf("UART Proto\n\n");
    printf("1 - Read  integer\n");
    printf("2 - Read  float\n");
    printf("3 - Read  string\n");
    printf("4 - Write integer\n");
    printf("5 - Write float\n");
    printf("6 - Write string\n\n");
    printf("7 - Exit\n\n");

    printf("Command: ");

    // uart in buffer int
    // uart in buffer float
    // uart in buffer string
}

int main() {
    const int INPUT_CAPACITY = 256;
    char input[INPUT_CAPACITY] = "\0";

    int uart_int = 0;
    while (1) {
        clear();
        print_ui();

        fgets(input, INPUT_CAPACITY, stdin);

        int command = atoi(input);
        if (command < 1 || command > 7) {
            printf("Wrong command: %d\n", command);
            msleep(2000);
        }

        switch (command) {
        case MENU_READ_INT:
            if (uart_proto_read_int(&uart_int)) {
                uart_proto_perror();
            }

            printf("Got integer: %d\n", uart_int);
            break;

        case MENU_WRITE_INT:
            fgets(input, INPUT_CAPACITY, stdin);

            uart_int = atoi(input);
            printf("Writing: %d\n", uart_int);

            if (uart_proto_write_int(uart_int)) {
                uart_proto_perror();
            }
            break;

        case MENU_EXIT:
            exit(0);
        default:
            break;
        }

        msleep(1000);
    }

    return 0;
}
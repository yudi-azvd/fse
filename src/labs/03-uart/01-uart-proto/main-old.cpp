#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#include "kbhit2.h"
#include "uart_proto.h"

#define clear() printf("\033[H\033[J")

const int BUFFER_CAPACITY = 4;
char buffer[BUFFER_CAPACITY] = "\0";
int buffer_size = 0;

void print_ui() {
    clear();
    printf("UART Proto\n\n");
    printf("1 - Read  integer\n");
    printf("2 - Read  float\n");
    printf("3 - Read  string\n");
    printf("4 - Write integer\n");
    printf("5 - Write float\n");
    printf("6 - Write string\n\n");

    printf("input buffer [");
    for (int i = 0; i < buffer_size; i++) {
        printf("%c", buffer[i]);
    }
    printf("], buffer size = %d\n", buffer_size);
    // uart in buffer int
    // uart in buffer float
    // uart in buffer string
}

void terminate(int sig) {
    kbhit2_restore_settings();
    exit(0);
}

// https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main() {
    kbhit2_get_default_settings();

    signal(SIGINT, terminate);
    signal(SIGHUP, terminate);

    while (1) {
        char c = '\0';
        if ((c = kbhit2())) {
            switch (c) {
            case K_BACKSPACE:
                if (buffer_size - 1 < 0)
                    continue;
                buffer_size--;
                buffer[buffer_size] = '\0';
                break;
            case K_1:
                int result = -1;
                if (uart_proto_read_int(&result) < 0) {
                    uart_proto_perror();
                }
            case K_2:
            case K_3:
            case K_4:
            case K_5:
            case K_6:
                if (buffer_size >= BUFFER_CAPACITY)
                    continue;
                buffer[buffer_size] = c;
                buffer_size++;
                break;
            default:
                break;
            }
        }

        print_ui();
        msleep(20);
    }

    kbhit2_restore_settings();
    return 0;
}
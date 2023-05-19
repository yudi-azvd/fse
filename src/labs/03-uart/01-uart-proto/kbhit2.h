#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

typedef enum {
    K_BACKSPACE = 127,
    K_DELETE = 8,
    K_ENTER = 10,
    K_1 = '1',
    K_2 = '2',
    K_3 = '3',
    K_4 = '4',
    K_5 = '5',
    K_6 = '6',
} Keys;

struct termios _old_settings;
struct termios _new_settings;

void kbhit2_get_default_settings() {
    tcgetattr(fileno(stdin), &_old_settings);
    _new_settings = _old_settings;
    _new_settings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &_new_settings);
}

//  Stack overflow
// https://stackoverflow.com/questions/18281412/check-keypress-in-c-on-linux
char kbhit2() {
    fd_set set;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 100'000;

    FD_ZERO(&set);
    FD_SET(fileno(stdin), &set);

    int res = select(fileno(stdin) + 1, &set, NULL, NULL, &tv);

    char c = '\0';
    if (res > 0) {
        read(fileno(stdin), &c, 1);
        // LOG_S("Input available: [%c]\n", c);
    } else if (res < 0) {
        // LOG_E("select error: %s\n", errormsg());
    } else {
        // LOG_W("Select timeout\n");
    }

    return c;
}

void kbhit2_restore_settings() {
    tcsetattr(fileno(stdin), TCSANOW, &_old_settings);
}

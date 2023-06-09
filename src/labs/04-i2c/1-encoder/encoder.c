// Adaptado de
// https://github.com/phil-lavin/raspberry-pi-gpio-interrupt

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <wiringPi.h>

// Which GPIO pin we're using
// How much time a change must be since the last in order to count as a change
#define IGNORE_CHANGE_BELOW_USEC 10'000

#define ENC_CW  1
#define ENC_CCW 0
int prev_dt_cl = 0;
int enc_direction;
long int enc_turn = 0;
int prev_cl = 0;

int ENC_PIN_DT;
int ENC_PIN_CL;
int ENC_PIN_SW;

static volatile int enc_switch;

struct timeval enc_last_turn;
struct timeval enc_last_switch;

void handle_switch() {
    struct timeval now;
    gettimeofday(&now, NULL);

    unsigned long diff;

    diff = (now.tv_sec * 1000000 + now.tv_usec) - (enc_last_switch.tv_sec * 1000000 + enc_last_switch.tv_usec);

    if (diff > IGNORE_CHANGE_BELOW_USEC) {
        printf("SW released\n");

        enc_switch = !enc_switch;
    }

    enc_last_switch = now;
}

void handle_encoder_turn();
void encoder_init(int pin_dt, int pin_cl, int pin_sw) {
    gettimeofday(&enc_last_turn, NULL);
    gettimeofday(&enc_last_switch, NULL);

    ENC_PIN_CL = pin_cl;
    ENC_PIN_DT = pin_dt;
    ENC_PIN_SW = pin_sw;

    pinMode(pin_cl, INPUT);
    pullUpDnControl(pin_cl, PUD_UP);
    pinMode(pin_dt, INPUT);
    pullUpDnControl(pin_dt, PUD_UP);
    pinMode(pin_sw, INPUT);
    pullUpDnControl(pin_sw, PUD_UP);

    wiringPiISR(pin_sw, INT_EDGE_RISING, &handle_switch);
    // wiringPiISR(pin_dt, INT_EDGE_BOTH, &handle_encoder_turn);
    wiringPiISR(pin_cl, INT_EDGE_BOTH, &handle_encoder_turn);

    int cl = digitalRead(ENC_PIN_CL);
    int dt = digitalRead(ENC_PIN_DT);

    prev_cl = cl;
    prev_dt_cl = dt << 1 | cl;
}

// void handle_encoder_turn() {
//     struct timeval now;
//     gettimeofday(&now, NULL);
//     unsigned long time_value = now.tv_sec * 1'000'000 + now.tv_usec;

//     int cl = digitalRead(ENC_PIN_CL);
//     int dt = digitalRead(ENC_PIN_DT);
//     int new_dt_cl = dt << 1 | cl;
//     // printf("  %lu | prev %x, new %x\n", time_value, prev_dt_cl, new_dt_cl);
//     if (new_dt_cl == prev_dt_cl)
//         return;

//     // Pra descobrir esse padrão, basta girar o encoder
//     int transition = (prev_dt_cl << 2) | new_dt_cl;
//     switch (transition) {
//     case 0b0111:
//     case 0b1110:
//     case 0b1000:
//     case 0b0001:
//         enc_direction = ENC_CW;
//         enc_turn++;
//         break;
//     case 0b1011:
//     case 0b1101:
//     case 0b0100:
//     case 0b0010:
//         enc_direction = ENC_CCW;
//         enc_turn--;
//         break;
//     default:
//         printf("??\n");
//         break;
//     }

//     char dir = enc_direction == ENC_CW ? 'H' : 'A';
//     printf("Girou %c! %d %d. Turns %d\n", dir, dt, cl, enc_turn);
//     // printf("Girou! %d %d\n", dt, cl);
//     prev_dt_cl = new_dt_cl;
// }

int events = 0;
void handle_encoder_turn() {
    int cl = digitalRead(ENC_PIN_CL);
    int dt = digitalRead(ENC_PIN_DT);

    if (cl != prev_cl) {
        if (dt != cl) {
            enc_turn++;
            enc_direction = ENC_CW;
        } else {
            enc_turn--;
            enc_direction = ENC_CCW;
        }
    }

    printf("turns %d, events %d, dir %d \n", enc_turn, events, enc_direction);

    prev_cl = cl;
}

int main() {
    if (wiringPiSetup()) {
        printf("wirinPi setup: failed");
        exit(1);
    }

    encoder_init(8, 9, 7);

    for (;;)
        delay(1);
}
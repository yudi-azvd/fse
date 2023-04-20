#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_1 RPI_V2_GPIO_P1_29

void handle_interruption(int sinal) {
    bcm2835_close();
    printf("closing program...\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (!bcm2835_init())
        exit(1);

    bcm2835_gpio_fsel(LED_1, BCM2835_GPIO_FSEL_OUTP);

    signal(SIGINT, handle_interruption);

    while (1) {
        bcm2835_gpio_write(LED_1, LOW);
        delay(1000);

        bcm2835_gpio_write(LED_1, HIGH);
        delay(2000);
    }
}

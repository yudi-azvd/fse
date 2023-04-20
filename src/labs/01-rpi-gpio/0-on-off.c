#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_1 RPI_V2_GPIO_P1_29

#define BUTTN RPI_V2_GPIO_P1_21

void configure_pins() {
    bcm2835_gpio_fsel(BUTTN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BUTTN, BCM2835_GPIO_PUD_DOWN);

    bcm2835_gpio_fsel(LED_1, BCM2835_GPIO_FSEL_OUTP);
}

void handle_interruption(int sinal) {
    bcm2835_close();
    printf("closing program...\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (!bcm2835_init())
        exit(1);

    configure_pins();
    signal(SIGINT, handle_interruption);

    while (1) {
        if (!bcm2835_gpio_lev(BUTTN)) {
            bcm2835_gpio_write(LED_1, LOW);
        } else
            bcm2835_gpio_write(LED_1, HIGH);

        delay(1500);
    }
}

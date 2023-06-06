#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>

#define BPM_DATA RPI_V2_GPIO_P1_03
#define BPM_CLK  RPI_V2_GPIO_P1_05

void handle_interruption(int sinal) {
    bcm2835_close();
    printf("closing program...\n");
    exit(0);
}

int main() {
    if (!bcm2835_init()) {
        printf("Erro ini BCM");
        exit(1);
    }

    signal(SIGINT, handle_interruption);

    bcm2835_gpio_fsel(BPM_DATA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BPM_DATA, BCM2835_GPIO_PUD_DOWN);

    bcm2835_gpio_fsel(BPM_CLK, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BPM_CLK, BCM2835_GPIO_PUD_DOWN);

    while (1) {
        delay(50);
    }

    return 0;
}
#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define clear() printf("\033[H\033[J")

#define ENCODER_SW  RPI_V2_GPIO_P1_16
#define ENCODER_DT  RPI_V2_GPIO_P1_18
#define ENCODER_CLK RPI_V2_GPIO_P1_22

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

    bcm2835_gpio_fsel(ENCODER_SW, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(ENCODER_SW, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_fsel(ENCODER_DT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(ENCODER_DT, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_fsel(ENCODER_CLK, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(ENCODER_CLK, BCM2835_GPIO_PUD_DOWN);

    while (1) {
        uint8_t enc_dt = bcm2835_gpio_lev(ENCODER_DT);
        uint8_t enc_sw = bcm2835_gpio_lev(ENCODER_SW);
        uint8_t enc_clk = bcm2835_gpio_lev(ENCODER_CLK);

        clear();
        printf("enc_dt  %d\n", enc_dt);
        printf("enc_sw  %d\n", enc_sw);
        printf("enc_clk %d\n", enc_clk);

        delay(10);
    }

    return 0;
}
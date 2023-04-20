#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <bcm2835.h>

#define LED_1 RPI_V2_GPIO_P1_33
#define BLINKING RPI_27

#define BTN_1 RPI_V2_GPIO_P1_19
#define BTN_2 RPI_V2_GPIO_P1_23

void handle_interruption(int sinal)
{
    bcm2835_close();
    printf("closing program...\n");
    exit(0);
}

int main(int argc, char **argv)
{
    if (!bcm2835_init())
        exit(1);

    bcm2835_gpio_fsel(BTN_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(BTN_2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BTN_1, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_set_pud(BTN_2, BCM2835_GPIO_PUD_DOWN);
    
    bcm2835_gpio_fsel(LED_1, BCM2835_GPIO_FSEL_OUTP);

    signal(SIGINT, handle_interruption);
    printf("program started\n");

    while (1)
    {
        uint8_t value = bcm2835_gpio_lev(BTN_1);

        printf("buttons (1 2): (%d %d)\n", 
            bcm2835_gpio_lev(BTN_1),
            bcm2835_gpio_lev(BTN_2)
            );
        if (bcm2835_gpio_lev(BTN_1))
            bcm2835_gpio_write(LED_1, HIGH);
        else
            bcm2835_gpio_write(LED_1, LOW);

        delay(1000);
    }
}

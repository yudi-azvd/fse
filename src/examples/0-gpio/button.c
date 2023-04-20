#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <bcm2835.h>

// Fonte:
// https://gitlab.com/fse_fga/gpio-raspberry-pi/-/blob/master/c/led_botao.c

/*
 * Configuração dos pinos dos LEDs e Botão
 */
#define LED_1 RPI_V2_GPIO_P1_40 // BCM 21
#define LED_2 RPI_V2_GPIO_P1_38 // BCM 20
#define LED_4 RPI_V2_GPIO_P1_36 // BCM 16
#define LED_8 RPI_V2_GPIO_P1_32 // BCM 26
#define BOTAO RPI_V2_GPIO_P1_05 // BCM 3

void configure_pins()
{
    // Define botão como entrada
    bcm2835_gpio_fsel(BOTAO, BCM2835_GPIO_FSEL_INPT);
    // Configura entrada do botão como Pull-up
    bcm2835_gpio_set_pud(BOTAO, BCM2835_GPIO_PUD_UP);

    // Configura pinos dos LEDs como saídas
    bcm2835_gpio_fsel(LED_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_8, BCM2835_GPIO_FSEL_OUTP);
}

void activate_leds(int led_mask)
{
    bcm2835_gpio_write(LED_1, led_mask & 1);
    bcm2835_gpio_write(LED_2, led_mask & 2);
    bcm2835_gpio_write(LED_4, led_mask & 4);
    bcm2835_gpio_write(LED_8, led_mask & 8);
}

void handle_interruption(int sinal)
{
    activate_leds(0);
    bcm2835_close();
    exit(0);
}

int main(int argc, char **argv)
{

    if (!bcm2835_init())
        exit(1);

    configure_pins();

    signal(SIGINT, handle_interruption);

    int counter;

    while (1)
    {
        counter = 0;
        while (counter < 16)
        {
            while (!bcm2835_gpio_lev(BOTAO))
            {
                printf("Botão reset pressionado\n");
                activate_leds(15);
                counter = 0;
            }

            activate_leds(counter);
            counter++;
            delay(1000);
        }
    }
}

#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_1 RPI_V2_GPIO_P1_29
#define LED_2 RPI_V2_GPIO_P1_31
#define LED_4 RPI_V2_GPIO_P1_33
#define LED_8 RPI_V2_GPIO_P1_35

#define BTN_1 RPI_V2_GPIO_P1_21
#define BTN_2 RPI_V2_GPIO_P1_19

#define ALL_LEDS_ON 0xFF
#define ALL_LEDS_OFF 0x00

#define LEDS_COUNT 4
uint8_t leds[LEDS_COUNT];

void configure_pins() {
    bcm2835_gpio_fsel(BTN_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BTN_1, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_fsel(BTN_2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BTN_2, BCM2835_GPIO_PUD_DOWN);

    bcm2835_gpio_fsel(LED_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LED_8, BCM2835_GPIO_FSEL_OUTP);
}

void handle_interruption(int sinal) {
    bcm2835_close();
    printf("closing program...\n");
    exit(0);
}

void activate_led(int counter) {
    bcm2835_gpio_write(LED_1, counter & 1);
    bcm2835_gpio_write(LED_2, counter & 2);
    bcm2835_gpio_write(LED_4, counter & 4);
    bcm2835_gpio_write(LED_8, counter & 8);
}

int main(int argc, char** argv) {
    if (!bcm2835_init())
        exit(1);

    configure_pins();
    printf("hey\n");

    signal(SIGINT, handle_interruption);
    activate_led(15);

    uint8_t counter = 1;
    uint8_t should_leds_turn_on = 0;
    while (1) {
        // Deveria ser um click ao invés de estado permanente como um push button
        // em vez de switch
        if (bcm2835_gpio_lev(BTN_1)) {
            printf("button 1 is pressed!\n");
            counter = counter << 1;
            if (counter > 8) {
                counter = 1;
            }
        }

        if (bcm2835_gpio_lev(BTN_2)) {
            printf("button 2 is pressed!\n");
            should_leds_turn_on = ALL_LEDS_ON;
        } else
            should_leds_turn_on = ALL_LEDS_OFF;

        printf("counter: %u\n", counter);
        printf("should:  %u\n", should_leds_turn_on);
        activate_led(counter & should_leds_turn_on);
        delay(3000);
    }
}

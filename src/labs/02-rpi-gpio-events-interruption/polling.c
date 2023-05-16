// Código adaptado do Livro: Raspberry Pi And The IoT In C
// Autor: Harry Fairhead
// Disponível em: https://www.iot-programmer.com/index.php/books/22-raspberry-pi-and-the-iot-in-c/chapters-raspberry-pi-and-the-iot-in-c/55-raspberry-pi-and-the-iot-in-c-input-and-interrupts?start=1
//
#include <bcm2835.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define BOTAO RPI_V2_GPIO_P1_36

int main(int argc, char** argv) {

    // Define a prioridade do programa / thread como máxima
    const struct sched_param priority = {1};
    sched_setscheduler(0, SCHED_FIFO, &priority);
    // Trava o processo na memória para evitar SWAP
    mlockall(MCL_CURRENT | MCL_FUTURE);

    if (!bcm2835_init())
        return 1;

    bcm2835_gpio_fsel(BOTAO, BCM2835_GPIO_FSEL_INPT);

    volatile int i;
    while (1) {
        while (1 == bcm2835_gpio_lev(BOTAO))
            ;
        while (0 == bcm2835_gpio_lev(BOTAO))
            ;
        for (i = 0; i < 5000; i++) {
            if (0 == bcm2835_gpio_lev(BOTAO))
                break;
        }
        printf("%d\n\r", i);
        fflush(stdout);
    }

    return (EXIT_SUCCESS);
}
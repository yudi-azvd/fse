#include <fcntl.h> //Used for UART
#include <stdio.h>
#include <string.h>
#include <termios.h> //Used for UART
#include <unistd.h>  //Used for UART

/**
 * Dúvidas memcpy(unsigned char*, float*, 4) resulta em segfault
*/

int main(int argc, const char* argv[]) {
    int uart0_filestream = -1;

    // Open in non blocking read/write mode
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_filestream == -1) {
        printf("Erro - Não foi possível iniciar a UART.\n");
    } else {
        printf("UART inicializada!\n");
    }
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char* p_tx_buffer;

    union _float_bytes {
        float value;
        unsigned char bytes[sizeof(float)];
    } float_bytes;

    float phi = 1.618;
    float pi = 3.14;
    float x = 0x1.0p-3;
    float one = 1.0;
    float_bytes.value = phi;
    for (int i = 0; i < sizeof(float); i++)
        printf("Byte %d: %.2x\n", i, float_bytes.bytes[i]);
    printf("%.4f\n", float_bytes.value);
    float_bytes.value = pi;
    for (int i = 0; i < sizeof(float); i++)
        printf("Byte %d: %.2x\n", i, float_bytes.bytes[i]);
    printf("%.4f\n", float_bytes.value);
    // p_tx_buffer = float_bytes.bytes;
    p_tx_buffer = (unsigned char*)&phi;

    // printf("memcpy p_tx_buffer <- pi. %d\n", sizeof(pi));
    // memcpy(p_tx_buffer, (unsigned char*)&pi, sizeof(pi));
    // printf("memcpy p_tx_buffer <- pi\n");

    // p_tx_buffer = &tx_buffer[0];
    // *p_tx_buffer++ = 'H';
    // *p_tx_buffer++ = 'e';
    // *p_tx_buffer++ = 'l';
    // *p_tx_buffer++ = 'l';
    // *p_tx_buffer++ = 'o';

    printf("Buffers de memória criados!\n");

    if (uart0_filestream != -1) {
        printf("Escrevendo caracteres na UART ...");
        // int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        // int count = write(uart0_filestream, &pi, sizeof(pi));
        int count = write(uart0_filestream, p_tx_buffer, sizeof(pi));
        if (count < 0) {
            printf("UART TX error\n");
        } else {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1) {
        // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        float recv_pi = -1.0;
        // int rx_length = read(uart0_filestream, (void*)rx_buffer, 255); // Filestream, buffer to store in, number of bytes to read (max)
        // int rx_length = read(uart0_filestream, &recv_pi, sizeof(pi));
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(pi));
        if (rx_length < 0) {
            printf("Erro na leitura.\n"); // An error occured (will occur if there are no bytes)
        } else if (rx_length == 0) {
            printf("Nenhum dado disponível.\n"); // No data waiting
        } else {
            // Bytes received
            // rx_buffer[rx_length] = '\0';
            // printf("%i Bytes lidos : %s\n", rx_length, rx_buffer);
            // rx_buffer[rx_length] = '\0';
            // printf("Received\n");
            // for (size_t i = 0; i < sizeof(float); i++) {
            //     printf("byte %d: %d\n", i, rx_buffer[i]);
            // }

            memcpy(&recv_pi, rx_buffer, sizeof(recv_pi));
            printf("float lido: pi = %.2f\n", recv_pi);
        }
    }

    close(uart0_filestream);
    return 0;
}

#include "uart_proto.h"

#include <stdio.h>

int main() {
    char* str = "uma string mt louca";
    WriteStrPacket p = WriteStrPacket_init(str);

    printf("size of WriteStrPacket = %ld\n", sizeof(WriteStrPacket));
    printf("   WriteStrPacket.size = %ld\n", p.size);
    printf("   WriteStrPacket.size = %ld\n", sizeof(p.signature));
    return 0;
}
#include "assembly_inline.hpp"

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb {%[port], %[retreg] | %[retreg], %[port]}"
                    : [retreg]"=a"(ret)
                    : [port]"Nd"(port) );
    return ret;
}

void outb(uint16_t port, uint8_t byte) {
    asm volatile ( "outb {%[byte], %[port] | %[port], %[byte]}"
                    : 
                    : [byte]"a"(byte), 
                      [port]"Nd"(port) );
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile ( "outw %0, %1"
                   :
                   : "a"(data), "Nd"(port) );
}

void io_wait() {
    outb(0x80, 0);
}
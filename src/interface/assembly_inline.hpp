#ifndef ASSEMBLY_INLINE_HPP
#define ASSEMBLY_INLINE_HPP

/**
 *  Interface for Using Assembly Instructions in C Code
 * 
 *  https://wiki.osdev.org/Inline_Assembly/Examples#I.2F=_access
 *  Stack Overflow with Title: "Porting AT&T inline-asm inb / outb wrappers to work with gcc -masm=intel"
*/


#include <inttypes.h>

// Recieve x Bit on IO-Location with Port
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

// Send out x Bit on IO-Location with Port
void outb(uint16_t port,  uint8_t byte);
void outw(uint16_t port, uint16_t byte);
void outl(uint16_t port, uint32_t byte);

// Wait for a very small amount of Time (generally 1-4 us) by performing NOP IO Operation on unused Port
void io_wait();

#endif
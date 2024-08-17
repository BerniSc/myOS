#include "timer.hpp"
#include "io.hpp"

volatile uint32_t ticks = 0; 

void Timer::init_timer(uint32_t freq) {
    uint32_t divisor = PIT_FREQ / freq;
    outb(PIT_CONTROL_PORT, 0x36);                   // Command byte: Mode 3, LSB then MSB, channel 0
    outb(PIT_CHANNEL_0, divisor & 0xFF);            // Divisor low byte
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF);     // Divisor high byte
}

void Timer::mask_timer() {
    uint8_t mask = inb(0x21);
    mask |= (1 << 0);
    outb(0x21, mask);
}

void Timer::unmask_timer() {
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 0);
    outb(0x21, mask);
}

extern "C" void timer_interrupt_handler_isr() {
    ticks++;
    outb(0x20, 0x20); // Send EOI
}
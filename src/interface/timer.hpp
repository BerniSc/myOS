#ifndef TIMER_HPP
#define TIMER_HPP

#include "assembly_inline.hpp" 

#define PIT_CONTROL_PORT 0x43
#define PIT_CHANNEL_0 0x40
#define PIT_FREQ 1193182
#define TIMER_FREQ 100

extern "C" void timer_interrupt_handler_isr();
extern "C" void timer_handler_interrupt();

extern volatile uint32_t ticks;

struct Timer {
    void init_timer(uint32_t freq); 
    void mask_timer();
    void unmask_timer();
};


#endif
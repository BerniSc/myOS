#include "isrs.hpp"

char *exception_messages[] = {
    "Division by Zero!",
    "Debug...",
    "NMI"
};

extern "C" void div_zero_exception_handler_isr() {
    io::my_cout(io::VGA_COLOUR::COLOUR_RED, io::VGA_COLOUR::COLOUR_WHITE) << "KERNEL_PANIC... DIVISION BY ZERO DETECTED... SHUTING DOWN\n";
    
    for(;;) {

    }
    //outb(0x20, 0x20);
}

//void _interrupt_fault_handler(uint64_t id,)
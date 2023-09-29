#include "io.hpp"
#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"
#include "keyboard.hpp"

#include "interrupts.hpp"

extern "C" void kernel_main() {
    interrupt_controller intc;
    keyboard_driver kbd;
    io::my_cout(io::COLOUR_LIGHT_BLUE);

    intc.init_interrupt_data_table();
    // Load the Keyboard Interrupt Handler with its ISR-Number, the "Pointer" to the ASM Handler, its Code Segment and its Flags into the IDT  
    intc.load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x08, 0x8E);
    // After Connecting the ISR to the Handler the Keyboard Interrupt can be Activated    
    kbd.keyboard_init();

    io::my_cout(io::COLOUR_GREEN);
    intc.enable_interrupts();


    io::my_cout << "Welcome to my 64-bit kernel!" << io::OSTREAM_APPEND::endl;

    CursorController::disableCursor();

    //CursorController::updateCursorPosition(0,3);

    //CursorController::enableCursor(2,3);

    while(true) __asm__("hlt\n\t");
}
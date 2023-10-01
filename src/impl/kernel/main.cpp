#include "io.hpp"
#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"
#include "keyboard.hpp"

#include "isrs.hpp"

#include "interrupts.hpp"

/*************************************************************************************
 * 
 *         Definition of the Global Variables that are Used by the Kernel
 * 
 *************************************************************************************/
keyboard_driver my_keyboard_driver;


extern "C" void kernel_main() {
    interrupt_controller intc;
    io::my_cout(io::COLOUR_LIGHT_BLUE);

    //my_keyboard_driver.set_silent(true);

    intc.init_interrupt_data_table();  
    intc.load_idt_entry(0x00, (uint64_t) division_zero_handler_interrupt, 0x08, 0x8E);
    // Load the Keyboard Interrupt Handler with its ISR-Number, the "Pointer" to the ASM Handler, its Code Segment and its Flags into the IDT  
    intc.load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x08, 0x8E);

    // After Connecting the ISR to the Handler the Keyboard Interrupt can be Activated    
    my_keyboard_driver.keyboard_init();

    io::my_cout(io::COLOUR_GREEN);
    intc.enable_interrupts();


    io::my_cout << "Welcome to my 64-bit kernel!" << io::OSTREAM_APPEND::endl;

    CursorController::disableCursor();

    //CursorController::updateCursorPosition(0,3);

    //CursorController::enableCursor(2,3);

    char buffer[12];
    io::my_cin.operator>>(buffer);
    io::my_cout << "You have entered " << buffer << '\n'; 
    io::my_cin >> buffer;
    io::my_cout << "Entered: " << buffer << '\n';
    for(int i = 0; i < 10; i++) {
        io::my_cin >> buffer;
        io::my_cout << "Entered: " << buffer << '\n';
    }

    while(true) __asm__("hlt\n\t");
}
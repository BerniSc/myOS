#include "io.hpp"
#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"
#include "keyboard.hpp"

#include "isrs.hpp"

#include "interrupts.hpp"

#include "memory_manager.hpp"


//https://stackoverflow.com/questions/329059/what-is-gxx-personality-v0-for
void* __gxx_personality_v0;
void* _Unwind_Resume;

/*************************************************************************************
 * 
 *         Definition of the Global Variables that are Used by the Kernel
 * 
 *************************************************************************************/
keyboard_driver my_keyboard_driver;
interrupt_controller my_interrupt_ctl;


struct ntor_test {
    ntor_test() {
        io::my_cout << "I have been created...\n";
    }

    ~ntor_test() {
        io::my_cout << "I have been destroyed...\n";
    }

    void foo() {
        io::my_cout << "I Fooed...\n";
    }
};

extern "C" uint64_t* heap_start;
extern "C" uint64_t* heap_end;

extern "C" void kernel_main() {
    char input_buffer[32];
    
    io::my_cout(io::COLOUR_LIGHT_BLUE);

    // Initialize IDT for Adding the Interrupts
    my_interrupt_ctl.init_interrupt_data_table();
    // Load the first Exception Handler (Div/0 Exception) to test Exception-Handling 
    my_interrupt_ctl.load_idt_entry(0x00, (uint64_t) division_zero_handler_interrupt, 0x08, 0x8E);
    // Load the Keyboard Interrupt Handler with its ISR-Number, the "Pointer" to the ASM Handler, its Code Segment and its Flags into the IDT  
    my_interrupt_ctl.load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x08, 0x8E);

    // After Connecting the ISR to the Handler the Keyboard Interrupt can be Activated    
    my_keyboard_driver.keyboard_init();
    my_keyboard_driver.set_silent(true);

    my_interrupt_ctl.enable_interrupts();

    io::my_cout(io::COLOUR_LIGHT_BLUE, io::COLOUR_LIGHT_GRAY) << "Press enter to proceed...";
    io::my_cin >> input_buffer;

    my_keyboard_driver.set_silent(false);
    
    io::my_cout(io::COLOUR_GREEN, io::COLOUR_BLACK) << io::OSTREAM_APPEND::clear;

    io::my_cout << "Welcome to my 64-bit kernel!" << io::OSTREAM_APPEND::endl;

    io::my_cout << "Would you Like to have the Cursor enabled? yes/no" << io::OSTREAM_APPEND::endl;
    while(true) {
        io::my_cin >> input_buffer;
        if(string_comp(input_buffer, "yes")) {
            break;
        } else if(string_comp(input_buffer, "no")) {
            CursorController::disableCursor();
            break;
        } 
        io::my_cout << "What are you trying to Say? ";
    }

    {
        // ntor_test my_test;
        memory_manager my_mm(heap_start, 4096);
        //uint8_t* test = reinterpret_cast<uint8_t*> (my_mm.my_kmalloc(sizeof(uint8_t)));
        //my_mm.my_kfree(test);
        ntor_test* mem_ptr = reinterpret_cast<ntor_test*> (my_mm.my_kmalloc(sizeof(ntor_test)));
        mem_ptr->foo();
        my_mm.my_kfree(mem_ptr);
    }

    io::my_cout << "Okay, then please enter your Name: ";
    io::my_cin >> input_buffer;

    io::my_cout << "Well, hello " << input_buffer << io::OSTREAM_APPEND::endl;
    
    // For testing of the CursorController Interface
    //CursorController::disableCursor();
    //CursorController::updateCursorPosition(0,3);
    //CursorController::enableCursor(2,3);

    io::my_cout << "And now for the Input test: " << io::OSTREAM_APPEND::endl;

    char buffer[12];
    io::my_cin.operator>>(buffer);
    io::my_cout << "You have entered " << buffer << '\n'; 
    io::my_cin >> buffer;
    io::my_cout << "Entered: " << buffer << '\n';
    for(int i = 0; i < 10; i++) {
        char buffer_2[12];
        io::my_cin >> buffer_2;
        io::my_cout << "Entered: " << buffer_2 << '\n';
    }

    while(true) __asm__("hlt\n\t");
}
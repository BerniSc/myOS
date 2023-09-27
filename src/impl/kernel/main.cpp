#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"

#include "interrupts.hpp"

void keyboard_init() {
    uint8_t curent_mask_master = inb(0x21);
    outb(0x21, curent_mask_master & 0xFD);
    print_str("Initied\n");
}

extern "C" void keyboad_handler_isr() {
    signed char keycode;
    print_char('a');

    keycode = inb(0x60);

    if(keycode >= 0) {
        print_char('a');
    }

    outb(0x20, 0x20);
}

extern "C" void kernel_main() {
    print_clear();

    init_interrupt_data_table();
    //ORIGINAL load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x08, 0x8E);
    load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x30, 0x8E);
    keyboard_init();

    //CursorController::disableCursor();
    CursorController::updateCursorPosition(0,3);
    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    print_str("Welcome to our 64-bit kernel!\n");
    //char buffer[10];
    //itoa(CursorController::getCursorX(CursorController::getCursorPosition()), buffer);
    //print_str(buffer);
    //itoa(CursorController::getCursorY(CursorController::getCursorPosition()), buffer);
    //print_str(buffer);
    //CursorController::enableCursor(2,3);

    while(true) __asm__("hlt\n\t");
}
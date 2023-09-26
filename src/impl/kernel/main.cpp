#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"

extern "C" void kernel_main() {
    print_clear();
    //CursorController::disableCursor();
    CursorController::updateCursorPosition(15,2);
    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    print_str("Welcome to our 64-bit kernel!\n");
    char buffer[10];
    itoa(CursorController::getCursorX(CursorController::getCursorPosition()), buffer);
    print_str(buffer);
    itoa(CursorController::getCursorY(CursorController::getCursorPosition()), buffer);
    print_str(buffer);
    //CursorController::enableCursor(2,3);

}
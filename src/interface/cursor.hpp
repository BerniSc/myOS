#ifndef CURSOR_HPP
#define CURSOR_HPP

/**
 *  Interface for Controlling the Keyboard Cursor in the OS Terminal
 * 
 *  https://wiki.osdev.org/Text_Mode_Cursor
 *  Stack Overflow with Title: "Porting AT&T inline-asm inb / outb wrappers to work with gcc -masm=intel"
*/

#include <stdint.h>
#include <stddef.h>

#include "assembly_inline.hpp"
#include "common_config.hpp"

#include "io.hpp"

struct CursorController {
    public:
        // Disable the Blinking Keyboard Cursor
        static void disableCursor();
        // Enable the Blinking Keyboard Cursor and Passing the start and end scanlines
        // scanlines := rows where the cursor Starts and Ends
        // highest Scanline := 0 and lowest scanline := maximum scanline (usually 15) 
        static void enableCursor(const uint8_t cursor_start, const uint8_t cursor_end);
        // Updating the Cursor Position
        static void updateCursorPosition(const uint8_t x, const uint8_t y);
        // Getting the Current Cursor-Position and its X and Y Components
        static uint16_t getCursorPosition();
        static uint8_t  getCursorX(const uint16_t cursorPos);
        static uint8_t  getCursorY(const uint16_t cursorPos);
    private:
        //Should be used via Static Instance
        CursorController() = delete;

};



#endif
#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <inttypes.h>

#include "assembly_inline.hpp"
#include "utils.hpp"
#include "io.hpp"

#include "keys.hpp"

#include "print.hpp"

// Interface to the C++ Handler Function
extern "C" void keyboad_handler_isr();
// Interface to the ASM Handler
extern "C" void keyboard_handler_interrupt();

struct keyboard_driver {
    private:
        static bool silent_mode;
    
    public:
        void keyboard_init();

        // Sets the Silent Mode for the Keyboard (Are Keypresses displayed or visually supressed)
        void set_silent(bool silent);
        bool get_silent() const;

        static char get_ascii_lower(uint8_t keycode);
        static char get_ascii_upper(uint8_t keycode);
};

extern keyboard_driver my_keyboard_driver;

#endif
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

namespace modifier_offset {
    constexpr uint8_t MODIFIER_SHIFT  = 0;
    constexpr uint8_t MODIFIER_CTRL   = 1;
    constexpr uint8_t MODIFIER_ALT    = 2;
    constexpr uint8_t MODIFIER_ESC    = 3;
}

struct keyboard_driver {
    private:
        static bool silent_mode;
        static uint8_t constexpr uppercase_dif = 'a' - 'A';

    public:
        void keyboard_init();

        // Sets the Silent Mode for the Keyboard (Are Keypresses displayed or visually supressed)
        void set_silent(bool silent);
        bool get_silent() const;

        static char keycode2ascii(uint8_t keycode, uint8_t modifiers = 0);
        static char get_ascii_upper(uint8_t keycode);
};

extern keyboard_driver my_keyboard_driver;

#endif
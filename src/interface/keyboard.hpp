#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <inttypes.h>

#include "assembly_inline.hpp"
#include "utils.hpp"
#include "io.hpp"

#include "keys.hpp"

#include "print.hpp"

extern "C" void keyboad_handler_isr();
extern "C" void keyboard_handler_interrupt();

struct keyboard_driver {
    void keyboard_init();

    static char get_ascii_lower(uint8_t keycode);
    static char get_ascii_upper(uint8_t keycode);
};

#endif
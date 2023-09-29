#ifndef IO_HPP
#define IO_HPP

#include <inttypes.h>
#include <cstddef>

#include "print.hpp"
#include "utils.hpp"

// Struct Definition for the VGA Character
struct vga_char {
    uint8_t character;
    uint8_t color;
};

namespace io {
    // Lenght and Height of VGA Terminal
    constexpr static size_t NUM_COLS = 80;
    constexpr static size_t NUM_ROWS = 25;

    enum VGA_COLOUR {
        COLOUR_BLACK = 0,
    	COLOUR_BLUE = 1,
    	COLOUR_GREEN = 2,
    	COLOUR_CYAN = 3,
    	COLOUR_RED = 4,
    	COLOUR_MAGENTA = 5,
    	COLOUR_BROWN = 6,
    	COLOUR_LIGHT_GRAY = 7,
    	COLOUR_DARK_GRAY = 8,
    	COLOUR_LIGHT_BLUE = 9,
    	COLOUR_LIGHT_GREEN = 10,
    	COLOUR_LIGHT_CYAN = 11,
    	COLOUR_LIGHT_RED = 12,
    	COLOUR_PINK = 13,
    	COLOUR_YELLOW = 14,
    	COLOUR_WHITE = 15,
    };

    enum OSTREAM_APPEND {
        endl = 0,
        clear = 1,
    };

    // Own Implementation of ostream
    struct my_ostream {
        private:
            vga_char* vga_buffer;

            VGA_COLOUR colour_fg;
            VGA_COLOUR colour_bg;

            // Current Position of "Pointer" in VGA Memory
            size_t current_col;
            size_t current_row;

            // Internal Functions for Accessing Video Memory
            void intern_clear_row(size_t row);
            void intern_print_clear();
            void intern_print_newline();
            void intern_print_char(char character);
            void intern_print_str(char* string);
            void intern_print_set_color(uint8_t foreground, uint8_t background);
        public:
            void my_ostream_init();

            // Output of C-String
            my_ostream& operator<<(char* string_buffer);
            // Output of Char
            my_ostream& operator<<(char character);
            // Output of int
            my_ostream& operator<<(uint32_t integer);
            my_ostream& operator<<(int32_t integer);
            // Output of long
            my_ostream& operator<<(uint64_t integer);
            my_ostream& operator<<(int64_t integer);
            // Handling of the OStream Appends
            my_ostream& operator<<(OSTREAM_APPEND os_append);

            my_ostream& operator()(VGA_COLOUR colour_foreground, VGA_COLOUR colour_background = COLOUR_BLACK);
    };

    // TODO Somehow this Does not Work only through the Constructor as it seems to not call the Constructor
    // For now this issue is solved by calling the Init Function in the kernelmain
    static my_ostream my_cout;

    // Own Implementation of istream
    struct my_cin {

    };
}

#endif
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

#define CIN_BUFFER_LENGTH 12

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
            static vga_char* vga_buffer;

            static VGA_COLOUR colour_fg;
            static VGA_COLOUR colour_bg;

            // Current Position of "Pointer" in VGA Memory
            static size_t current_col;
            static size_t current_row;

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
    // For now this issue is solved by calling the Init Function in the kernelmain to Reset and by Setting the Parameters as Static and 
    // then initializing them in the .cpp 
    static my_ostream my_cout;

    // Own Implementation of istream
    struct my_istream {
        private:
            static uint8_t buffer_pos; 
            
            static char recieved_character;
            static char termination_character;
            static bool has_recieved;

            void reset_buffer_pos();
            void reset_recieved_character();

            // Let the Keyboard driver access the Private Functions (i.e. operator<<()) of this Class
            friend class keyboard_driver;
        public:
            //sadly not yet Safe. For now it is assumend that this function is calld with a Char buffer with the size of at least CIN_BUFFER_LENGTH
            // The Other bits are disregarded. Otherwise there WILL BE AN UNCOMFORTABLE MEMORY EXCEPTION -->> TODO
            my_istream& operator>>(char string_buffer[12]);

            // Sadly for now has to be PUBLIC as ISR is not yet a Member function -> Must be accessible globaly TODO
            void operator<<(char character);
    };

    static my_istream my_cin;
}

#endif
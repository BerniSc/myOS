#include "io.hpp"

/**************************************************************************************
 *                      Internal Print-Function Implementations
 **************************************************************************************/
void io::my_ostream::intern_clear_row(size_t row) {
    vga_char empty = vga_char {
        character: ' ',
        color: (uint8_t(colour_fg) | (uint8_t(colour_bg) << 4))
    };

    for (size_t current_col = 0; current_col < io::NUM_COLS; current_col++) {
        vga_buffer[current_col + io::NUM_COLS * row] = empty;
    }
}

void io::my_ostream::intern_print_clear() {
    for (size_t i = 0; i < io::NUM_ROWS; i++) {
        intern_clear_row(i);
    }
    current_col = 0;
    current_row = 0;
}

void io::my_ostream::intern_print_newline() {
    current_col = 0;

    if (current_row < io::NUM_ROWS - 1) {
        current_row++;
        return;
    }

    for (size_t row = 1; row < io::NUM_ROWS; row++) {
        for (size_t col = 0; col < io::NUM_COLS; col++) {
            vga_char character = vga_buffer[col + io::NUM_COLS * row];
            vga_buffer[col + io::NUM_COLS * (row - 1)] = character;
        }
    }

    intern_clear_row(io::NUM_COLS - 1);
}

void io::my_ostream::intern_print_char(char character) {
    if (character == '\n') {
        intern_print_newline();
        return;
    }

    if (current_col > io::NUM_COLS) {
        intern_print_newline();
    }

    vga_buffer[current_col + io::NUM_COLS * current_row] = (vga_char) {
        character: (uint8_t) character,
        color: (uint8_t(colour_fg) | (uint8_t(colour_bg) << 4)),
    };

    current_col++;
}

void io::my_ostream::intern_print_str(char* str) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) str[i];

        if (character == '\0') {
            return;
        }

        intern_print_char(character);
    }
}

void io::my_ostream::intern_print_set_color(uint8_t foreground, uint8_t background) {
    //color_1 = foreground + (background << 4);
    colour_fg = static_cast<io::VGA_COLOUR>(foreground);
    colour_bg = static_cast<io::VGA_COLOUR>(background);
}

/**************************************************************************************
 *                              OStream Implementations
 **************************************************************************************/

vga_char* io::my_ostream::vga_buffer = reinterpret_cast<vga_char*>(0xb8000);

io::VGA_COLOUR io::my_ostream::colour_fg = io::COLOUR_WHITE;
io::VGA_COLOUR io::my_ostream::colour_bg = io::COLOUR_BLACK;

// Current Position of "Pointer" in VGA Memory
size_t io::my_ostream::current_col = 0;
size_t io::my_ostream::current_row = 0;

void io::my_ostream::my_ostream_init() {
    vga_buffer = reinterpret_cast<vga_char*>(0xb8000);

    colour_fg = io::COLOUR_WHITE;
    colour_bg = io::COLOUR_BLACK;

    current_row = 0;
    current_col = 0;

    intern_print_clear();
}

io::my_ostream& io::my_ostream::operator<<(char* string_buffer) {
    intern_print_str(string_buffer);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(char character) {
    intern_print_char(character);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(int32_t integer) {
    char string_buffer[12];
    itoa_t(integer, string_buffer);
    intern_print_str(string_buffer);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(uint32_t integer) {
    char string_buffer[12];
    itoa_t(integer, string_buffer);
    intern_print_str(string_buffer);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(int64_t integer) {
    char string_buffer[11];
    itoa_t(integer, string_buffer);
    intern_print_str(string_buffer);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(uint64_t integer) {
    char string_buffer[21];
    itoa_t(integer, string_buffer);
    intern_print_str(string_buffer);
    return *this;
}

io::my_ostream& io::my_ostream::operator<<(OSTREAM_APPEND os_append) {
    if(os_append == io::OSTREAM_APPEND::endl) {
        intern_print_newline();
    } else if(os_append == io::OSTREAM_APPEND::clear) {
        intern_print_clear();
    }
    return *this;
}

io::my_ostream& io::my_ostream::operator()(VGA_COLOUR colour_foreground, VGA_COLOUR colour_background) {
    intern_print_set_color(this->colour_fg = colour_foreground, this->colour_bg = colour_background);
    return *this;
}
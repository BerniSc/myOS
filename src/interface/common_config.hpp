#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/**
 *  Interface for Storing/Accessing Common Used Variables over multiple Files
*/

#include <inttypes.h>
#include <stddef.h>

namespace constants {
    inline constexpr size_t NUM_COLS_VGA = 80;
    inline constexpr size_t NUM_ROWS_VGA = 25;

    inline constexpr size_t INPUT_BUFFER_SIZE = 128; 
}

#endif
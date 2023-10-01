#ifndef UTILS_HPP
#define UTILS_HPP

#include <inttypes.h>

extern uint32_t strlen(const char* string);
extern uint32_t digit_count(int number);

extern void itoa(uint8_t number, char* buffer);

// Function for reversing a string destructivly in place
extern void reverse(char* str, uint32_t length);

bool string_comp(const char* string1, const char* string2);

/******************************************************************************************
 *
 *            Definition of Template Functions -> Have to be in Header
 * 
 ******************************************************************************************/

/*******************************************************************************
 *     Is-Signed Strcutures for checking wheter Template Parameter is signed
 *******************************************************************************/
template <typename T>
struct is_signed {
    static const bool value = T(-1) < T(0);
};

template <>
struct is_signed<char> {
    static const bool value = (char(-1) < char(0));
};

template <>
struct is_signed<signed char> {
    static const bool value = (static_cast<signed char>(-1) < static_cast<signed char>(0));
};

template <>
struct is_signed<unsigned char> {
    static const bool value = false;
};

/*******************************************************************************
 *         Function to convert an integer of arbitrary Size to a String
 *******************************************************************************/
template <typename T>
void itoa_t(T value, char* buffer, int base = 10) {
    // Handle 0 explicitly, otherwise empty String is printed
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    // Handle negative numbers for signed types
    bool isNegative = false;
    if (is_signed<T>::value && value < 0) {
        isNegative = true;
        value = -value;
    }

    // Process individual digits
    int i = 0;
    while (value != 0) {
        int rem = value % base;
        buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }

    // Add the negative sign for negative numbers
    if (isNegative) {
        buffer[i++] = '-';
    }

    // Null-terminate the string
    buffer[i] = '\0';

    // Reverse the string
    reverse(buffer, i);

    // If the value was zero, set the buffer to "0"
    if (i == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
    }
}


#endif
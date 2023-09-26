#ifndef UTILS_HPP
#define UTILS_HPP

#include <inttypes.h>

extern uint32_t strlen(const char* string);
extern uint32_t digit_count(int number);
extern void itoa(uint8_t number, char* buffer);

#endif
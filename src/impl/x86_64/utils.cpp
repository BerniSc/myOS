#include "utils.hpp"

uint32_t strlen(const char* str) {
    uint32_t length = 0;
    while(str[length])
        length++;
    return length;
}

uint32_t digit_count(int num) {
    uint32_t count = 0;
    if(num == 0)
        return 1;
    while(num > 0) {
        count++;
        num = num/10;
    }
    return count;
}

// Converting chars into Strings
void itoa(uint8_t num, char *number) {
    int dgcount = digit_count(num);
    int index = dgcount - 1;
    char x;
    if(num == 0 && dgcount == 1) {
        number[0] = '0';
        number[1] = '\0';
    } else {
        while(num != 0) {
            x = num % 10;
            number[index] = x + '0';
            index--;
            num = num / 10;
        }
        number[dgcount] = '\0';
    }
}

// Function for reversing a string destructivly in place
void reverse(char* str, uint32_t length) {
    uint32_t start = 0;
    uint32_t end = length - 1;

    while(start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}

bool string_comp(const char* string1, const char* string2) {
    while(*string1 != '\0' && *string2 != '\0') {
        if(*string1 != *string2) {
            return false;
        }
        string1++;
        string2++;
    }

    // Check if both Strings have reached their end
    return (*string1 == '\0' && *string2 == '\0');
}

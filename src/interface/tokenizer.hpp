#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <stddef.h>
#include "utils.hpp"
#include "memory_manager.hpp"
#include "ps_pointer.hpp"

class Tokenizer {
    public:
        Tokenizer(const char* token_string, const char symbol) : token_string(token_string), symbol(symbol), pos(0) {}

        inline const char* nextToken() {
            if(pos >= strlen(token_string))
                return nullptr;
            
            // Find the next occurence of Symbol
            size_t start = pos;
            size_t end = pos;

            while(token_string[end] != symbol && token_string[end] != '\0')
                ++end;
            
            // Extract the Token
            size_t length = end - start;
            char* token = new char[length + 1];
            kmemcpy(token_string + start, token, length);
            token[length] = '\0';

            // Update pos for the next Call
            pos = (token_string[end] == symbol) ? end + 1 : end;

            return token;
        }

    private:
        const char* token_string;
        const char symbol;
        size_t pos;
};

#endif
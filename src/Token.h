#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token {
    std::string Name;
    std::string lexeme;
    std::string pattern;
};

#endif
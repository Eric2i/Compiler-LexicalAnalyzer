#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token {
    std::string Name;
    std::string pattern;

    bool operator<(const Token& other) const {
        return this->Name > other.Name;
    }
};

#endif
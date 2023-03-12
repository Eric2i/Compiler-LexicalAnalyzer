#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token {
    int sequence;   // sequence number specified by input order
    std::string Name;
    std::string pattern;

    bool operator<(const Token& other) const {
        return this->sequence < other.sequence;
    }
};

#endif
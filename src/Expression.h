#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include <stack>
#include <set>
#include "NFA.h"
#include "Token.h"

typedef int NFAState;

struct Expression
{    
    std::string expression;
    std::string token_name;
    std::string postfix;
    NFA nfa;
    std::set<char> alphabet;
    std::map<NFAState, Token> tokens;

    void definition2regex();
    void in2post();
    bool PartialOrd(const char, const char);
    void ConstructNFA(int);

    bool NFASimulator(const std::string &);
    bool Match(const std::string&);
    void addState(int);
};

//DEBUG
void report_currentStates();
void show_alphabet(Expression &e);

#endif
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include <set>
#include "NFA.h"

struct Expression
{    
    std::string expression;
    std::string postfix;
    NFA nfa;

    void in2post();
    bool PartialOrd(const char, const char);
    void ConstructNFA();

    std::set<int> epsilonClousure(int);
    bool NFASimulator(const std::string &);
    void addState(int);

};

//DEBUG
void report_currentStates();

#endif
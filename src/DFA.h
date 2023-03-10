#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>
#include <utility>
#include <vector>
#include "FA.h"
#include "NFA.h"
#include "Expression.h"

struct DFA {
    std::set<char> alphabet;

    std::set<int> start;
    std::set<std::set<int>> accept;

    std::set<std::set<int>> Dstates;
    std::map<std::pair<std::set<int>, char>, std::set<int>> Dtrans;
    
    void NFA2DFA(NFA&);
};

std::set<int> epsilon_closure(std::set<int> T, NFA &N);
std::set<int> move(std::set<int> T, char a, NFA &N);

// DEBUG
void show_DFA(DFA &);
void print_Set(std::set<int> &S);
void show_alphabet(std::set<char>);

#endif
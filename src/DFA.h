#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>
#include <utility>
#include <vector>
#include "Expression.h"

typedef std::set<int> DFAState;

struct DFA {
    std::set<char> alphabet;

    DFAState start;
    std::set<DFAState> accept;

    std::set<DFAState> Dstates;
    std::map<std::pair<DFAState, char>, DFAState> Dtrans;
    
    std::map<DFAState, std::set<Token>> tokens;

    void NFA2DFA(NFA&);
    void MoveTokens(std::map<NFAState, Token> &);
    DFAState trans(DFAState&, char&);
};

DFA DFAMinimize(DFA &);
DFA Expression2DFA(Expression &e);
Expression mergeExpressions(std::stack<Expression>);
DFAState epsilon_closure(DFAState, NFA &N);
DFAState move(DFAState T, char a, NFA &N);
void set_partition(std::set<DFAState> states, int id);
void set_partition(DFAState s, int id);
void remove_partition_byID(int id);
std::set<std::set<DFAState>> subgroup(std::set<DFAState> G, DFA &D);

// DEBUG
void show_DFA(DFA &);
void show_tokens(DFA &);
void print_Set(const std::set<int> &);
void show_alphabet(std::set<char>);
void show_partition_table();
void subgrouping_report(std::set<DFAState>, std::set<std::set<DFAState> >);
void show_vectorization(std::map<std::vector<int>, std::set<DFAState>> &);
#endif
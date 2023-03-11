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
    
    void NFA2DFA(NFA&);
    DFAState trans(DFAState&, char&);
};

std::set<int> epsilon_closure(std::set<int> T, NFA &N);
std::set<int> move(std::set<int> T, char a, NFA &N);
Expression mergeExpressions(std::stack<Expression>);
DFA DFAMinimize(DFA &D);
void set_partition(std::set<DFAState> states, int id);
void set_partition(DFAState s, int id);
void remove_partition_byID(int id);
std::set<std::set<DFAState>> subgroup(std::set<DFAState> G, DFA &D);

// DEBUG
void show_DFA(DFA &);
void print_Set(std::set<int> &);
void show_alphabet(std::set<char>);
void show_partition_table();
void subgrouping_report(std::set<DFAState>, std::set<std::set<DFAState> >);
void show_vectorization(std::map<std::vector<int>, std::set<DFAState>> &);
#endif
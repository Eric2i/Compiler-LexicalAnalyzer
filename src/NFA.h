#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <map>
#include <vector>

struct Edge
{
    int dest;   // destination of the edge
    char label; // label of the edge
};

struct State
{
    std::vector<Edge> outEdges;
};

struct NFA
{
    int start;  // initial state
    int accept; // final state
    std::map<int, State> states;
};

NFA char2NFA(char c);
NFA Union(NFA Ns, NFA Nt);
NFA Concat(NFA Ns, NFA Nt);
NFA KleenClosure(NFA N);
NFA PositiveClosure(NFA N);
void reset_state_counter();

/*DEBUG*/
void show_NFA(NFA &);
int report_counter();
#endif
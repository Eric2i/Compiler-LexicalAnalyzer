#ifndef NFA_H
#define NFA_H

#include <map>
#include <vector>
#include "FA.h"

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
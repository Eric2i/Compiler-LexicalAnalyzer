#include "DFA.h"
#include <stack>
#include <iostream>

const char EPSILON = '\0';

void DFA::NFA2DFA(NFA &N) {
    this->start = epsilon_closure(std::set<int>({N.start}), N);    
    this->Dstates.insert(this->start);
    std::map<std::set<int>, bool> isMarked;
    while(isMarked.size() < this->Dstates.size()) {
        // there exists one unmarked state
        for(auto T: this->Dstates) {
            if(isMarked.find(T) == isMarked.end()) { 
                // found unmarked state T
                isMarked[T] = true;
                for(auto a: this->alphabet) {
                    std::set<int> U = epsilon_closure(move(T, a, N), N);
                    if(U.size() == 0) continue; // !exsist U=move(T,a), then nothing to do
                    if(this->Dstates.find(U) == this->Dstates.end()) {
                        this->Dstates.insert(U);
                        if(U.find(N.accept) != U.end()) this->accept.insert(U);
                    }
                    this->Dtrans[{T, a}] = U;
                }
            }
        }
    }
}

std::set<int> epsilon_closure(std::set<int> T, NFA &N) {
    std::stack<int> stk;
    for(auto i: T) stk.push(i);
    std::set<int> closure;
    while(!stk.empty()) {
        int t = stk.top();
        stk.pop();
        closure.insert(t);
        for(auto outEdges_t: N.states[t].outEdges) {
            if(outEdges_t.label == EPSILON && closure.find(outEdges_t.dest) == closure.end()) {
                closure.insert(outEdges_t.dest);
                stk.push(outEdges_t.dest);
            }
        }
    }

    // std::cerr << "Message from epsilon-closure: " << std::endl;
    // print_Set(closure);
    return closure;
}

std::set<int> move(std::set<int> T, char a, NFA &N) {
    std::set<int> afterMove;

    for(auto t: T) {
        for(auto outEdges_t: N.states[t].outEdges) { 
            if (outEdges_t.label == a) { 
                afterMove.insert(outEdges_t.dest);
            }
        }
    }
    return afterMove;
}

// DEBUG
void show_DFA(DFA &D) {
    std::cerr << "alphabet:" << std::endl;
    show_alphabet(D.alphabet);

    std::cerr << "Total states: " << D.Dstates.size() << std::endl;
    
    for(auto s: D.Dstates) {
        std::cerr << "{";
        for(auto i: s) {std::cerr << i << ", ";}
        std::cerr << "}, ";
    }
    std::cerr << std::endl;

    std::cerr << "start state: " << std::endl;
    std::cerr << "{";
    for(auto i: D.start) {std::cerr << i << ", ";}
    std::cerr << "}, ";
    std::cerr << std::endl;
    
    std::cerr << "accept states: " << std::endl;
    for(auto s: D.accept) {
        std::cerr << "{";
        for(auto i: s) {std::cerr << i << ", ";}
        std::cerr << "}, ";
    }
    std::cerr << std::endl;

    std::cerr << "valid transitions: " << std::endl;
    for(auto t: D.Dtrans) {
        std::cerr << "{";
        for(auto i: t.first.first) {std::cerr << i << ",";}
        std::cerr << "} -> " << t.first.second << " -> {";
        for(auto i: t.second) {std::cerr << i << ",";}
        std::cerr << "}"<<std::endl;
    }
}

// DEBUG
void print_Set(std::set<int> &S) {
    for(auto i: S) {std::cerr << i << ",";}
    std::cerr << std::endl;
}

// DEBUG
void show_alphabet(std::set<char> alphabet) { 
    for(auto i: alphabet) {
        std::cerr << i << " ";
    }
    std::cerr << std::endl;
}

Expression mergeExpressions(std::stack<Expression> exps) {
    Expression exp;
    exp.nfa.start = report_counter();
    exp.nfa.accept = report_counter()+1;
    
    while(!exps.empty()) { 
        auto e = exps.top();
        exps.pop();
        exp.alphabet.insert(e.alphabet.begin(), e.alphabet.end());
        
        exp.nfa.states[exp.nfa.start].outEdges.push_back({e.nfa.start, EPSILON});
        exp.nfa.states[e.nfa.accept].outEdges.push_back({exp.nfa.accept, EPSILON});
        exp.nfa.states.insert(e.nfa.states.begin(), e.nfa.states.end()); 
    }

    return exp;
}
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

DFAState DFA::trans(DFAState& s, char& c) {
    
    if(this->Dtrans.find({s, c}) != this->Dtrans.end()) {
        return this->Dtrans[{s, c}];
    }
    return DFAState();
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

std::map<DFAState, int> PartitionID;
std::map<int, std::set<DFAState>> Partition;
std::map<int, DFAState> Repr;

DFA DFAMinimize(DFA &D) {
    DFA dfa;
    dfa.alphabet = D.alphabet;
    int counter = 0;

    // Initialize partition
    PartitionID[DFAState({})] = -1;
    set_partition(D.accept, counter++);
    for(auto s: D.Dstates) if(D.accept.find(s) == D.accept.end()) set_partition(s, counter);
    ++counter;
    
    // show_partition_table();
    // Split
    bool splited;
    do{
        splited = false;
        for(auto P: Partition) {
            int ID = P.first;
            std::set<DFAState> G = P.second;
            auto subgroups = subgroup(G, D);
            // subgrouping_report(G, subgroups);
            if(subgroups.size() > 1) {
                // Update
                splited = true;
                remove_partition_byID(ID);
                for(auto s: subgroups) {
                    set_partition(s, counter++);
                }
                // show_partition_table();
                break;
            }
        }
    }while(splited);

    // std::cerr << "size of partition now: " << Partition.size() << std::endl;
    // choose representative
   for(auto s: Partition) {
        int id = s.first;
        Repr[id] = *s.second.begin();
        dfa.Dstates.insert(Repr[id]);
    }

    // Construct minimized dfa
    dfa.start = Repr[PartitionID[D.start]];
    for(DFAState s: D.accept) {
        dfa.accept.insert(Repr[PartitionID[s]]);
    }

    for(auto s: dfa.Dstates) {
        for(auto c: D.alphabet) {
            DFAState t = D.trans(s, c);
            if(t.size() > 0) dfa.Dtrans[{Repr[PartitionID[s]], c}] = Repr[PartitionID[t]];
        }
    }
    return dfa;
}

void set_partition(DFAState s, int id) {
    Partition[PartitionID[s]].erase(s);
    if(Partition[PartitionID[s]].size() == 0) Partition.erase(PartitionID[s]);
    PartitionID[s] = id;
    Partition[id].insert(s);
}

void set_partition(std::set<DFAState> states, int id) {
    for(auto s: states) {
        set_partition(s, id);
    }
}

void remove_partition_byID(int id) {
    Partition.erase(id);
} 

std::set<std::set<DFAState>> subgroup(std::set<DFAState> G, DFA &D) {
    std::map<std::vector<int>, std::set<DFAState>> subsets;
    for(auto g: G) {
        std::vector<int> v;
        for(auto c: D.alphabet) v.push_back(PartitionID[D.trans(g, c)]);
        subsets[v].insert(g);
    }
    // show_vectorization(subsets);
    std::set<std::set<DFAState>> subgroups;
    for(auto s: subsets) {
        subgroups.insert(s.second);
    }
    return subgroups;
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
    std::cerr << "{";
    for(auto i: S) {std::cerr << i << ",";}
    std::cerr << "}";
}

// DEBUG
void show_alphabet(std::set<char> alphabet) { 
    for(auto i: alphabet) {
        std::cerr << i << " ";
    }
    std::cerr << std::endl;
}

// DEBUG
void show_partition_table() {
    // print out partition and partitionID as a table
    std::cerr << "PartitionTb=====================================================" << std::endl;
    for(auto entry: Partition) {
        int ID = entry.first;
        std::set<DFAState> G = entry.second; 
        std::cerr << "Paritition " << ID << ": ";
        for(auto each: G) {
            print_Set(each); 
            std::cerr << ", ";
        }
            std::cerr << "\n";
    }
    std::cerr << "================================================================" << std::endl;
}

void subgrouping_report(std::set<DFAState> G, std::set<std::set<DFAState> > subgroups) {
    // subgrouping(G) -> subgroups
    std::cerr<<"SubgroupingReport============================================" << std::endl;
    std::cerr << "{";
    for(auto g: G) { print_Set(g); std::cerr << ",";}
    std::cerr << "} =>";
    for(auto group: subgroups) {
        std::cerr << "{";
        for(auto g: group) { print_Set(g); std::cerr << ",";}
        std::cerr << "};";
    }
    std::cerr << "\n";
    std::cerr<<"=============================================================" << std::endl;
}

void show_vectorization(std::map<std::vector<int>, std::set<DFAState>> &subsets) {
    std::cerr << "vectorizationReport================================" << std::endl;
    for(auto entry: subsets) {
        auto vector = entry.first;
        auto G = entry.second;
        std::cerr << "vector: [";
        for(auto i: vector) std::cerr << i << ",";
        std::cerr << "] => ";
        std::cerr << "{";
        for(auto g: G) {print_Set(g); std::cerr << ",";}
        std::cerr << "}\n";
    }
    std::cerr << "===================================================" << std::endl;
}
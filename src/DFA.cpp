#include "DFA.h"
#include <stack>
#include <iostream>
#include <fstream>

const char EPSILON = '\0';

DFA Expression2DFA(Expression &e) {
    DFA D;
    D.alphabet = e.alphabet;
    D.NFA2DFA(e.nfa);       
    D.MoveTokens(e.tokens);
    return D;
}

void DFA::NFA2DFA(NFA &N) {
    this->start = epsilon_closure(DFAState({N.start}), N);    
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

void DFA::MoveTokens(std::map<NFAState, Token> &Ntokens) {
    for(auto dfastate: this->Dstates) {
        for(auto entry: Ntokens) {
            NFAState nfastate = entry.first;
            Token token = entry.second;
            if(dfastate.find(nfastate)!= dfastate.end()) {
                if(this->tokens.find(dfastate) == this->tokens.end()) this->tokens[dfastate] = token;
                // Conflict Resolution
                else if(token < this->tokens[dfastate]) {
                    this->tokens[dfastate] = token;
                }
            }
        }
    }
}

DFAState epsilon_closure(DFAState T, NFA &N) {
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

DFAState move(DFAState T, char a, NFA &N) {
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

        exp.tokens.insert(e.tokens.begin(), e.tokens.end());
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

    /* Initialization */
    // Special Case, to fill the blankets in transition table
    PartitionID[DFAState({})] = -1; 
    // set_partition for final states accepting same token
    std::map<Token, std::set<DFAState>> token2states;
    for(auto s: D.tokens) {
        DFAState state = s.first;
        Token token = s.second;
        token2states[token].insert(state);
    }
    for(auto s: token2states) {
        set_partition(s.second, counter++);
    }
    // set_partiton for all non-final states
    for(auto s: D.Dstates) if(D.accept.find(s) == D.accept.end()) set_partition(s, counter);
    ++counter;
    
    /* spliting */
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

    /* choose representative */
   for(auto s: Partition) {
        int id = s.first;
        // always choose the first DFAState in one Group as representative
        Repr[id] = *s.second.begin();
        // minimized nfa only includes representatives from each group
        dfa.Dstates.insert(Repr[id]);
    }

    /* complete the minimized dfa */
    // define start states
    dfa.start = Repr[PartitionID[D.start]];
    // define final states
    for(DFAState s: D.accept) {
        dfa.accept.insert(Repr[PartitionID[s]]);
    }
    // define transition table
    for(auto s: dfa.Dstates) {
        for(auto c: D.alphabet) {
            DFAState t = D.trans(s, c);
            if(t.size() > 0) dfa.Dtrans[{Repr[PartitionID[s]], c}] = Repr[PartitionID[t]];
        }
    }

    // define token that each final state accept
    for(auto entry: D.tokens) {
        DFAState dfastate = entry.first;
        Token token = entry.second;
        dfa.tokens[Repr[PartitionID[dfastate]]] = token;
    }

    // return the minimized dfa
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

    std::map<DFAState, int> notations;
    std::cerr << "Total states: " << D.Dstates.size() << std::endl;
    
    for(auto s: D.Dstates) {
        notations[s] = notations.size();
        std::cerr << notations[s] << ": ";
        std::cerr << "{";
        for(auto i: s) {std::cerr << i << ",";}
        std::cerr << "}\n";
    }
    std::cerr << std::endl;

    std::cerr << "start state: " << std::endl;
    std::cerr << notations[D.start] << std::endl;
    
    std::cerr << "accept states: " << std::endl;
    for(auto s: D.accept) {
        std::cerr << notations[s] << " ";
    }
    std::cerr << std::endl;

    std::cerr << "valid transitions: " << std::endl;
    for(auto t: D.Dtrans) {
        // std::cerr << "{";
        // for(auto i: t.first.first) {std::cerr << i << ",";}
        // std::cerr << "} -> " << t.first.second << " -> {";
        // for(auto i: t.second) {std::cerr << i << ",";}
        // std::cerr << "}"<<std::endl;
        std::cerr << notations[t.first.first] << " --> " 
                << notations[t.second] 
                << ": " << t.first.second
                << std::endl;
    }
}

// DEBUG
void show_tokens(DFA &D) {
    std::map<DFAState, int> notations;
    for(auto s: D.Dstates) {
        notations[s] = notations.size();
    }
    std::cerr << "Tokens==================================================" << std::endl;
     for(auto t: D.tokens) {
        std::cerr << notations[t.first];
        std::cerr << ": ";
        // std::cerr << "{";
        std::cerr << t.second.Name;
        // std::cerr << "}";
        std::cerr << std::endl;
    }
    std::cerr << "=========================================================" << std::endl;
}

// DEBUG
void print_Set(const std::set<int> &S) {
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

// DEBUG
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

// DEBUG
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

// DEBUG
void make_mermaid(DFA &D) {
    std::ofstream fout("test/output/mermaid.txt");
    if(!fout) {
        std::cerr << "Failed to open the output file" << std::endl;
        return;
    }
    
    fout << "stateDiagram-v2\n";
    std::map<DFAState, int> notations;
    for(auto s: D.Dstates) {
        notations[s] = notations.size();
    }
    for(auto t: D.Dtrans) {
        fout << notations[t.first.first] << " --> " 
                << notations[t.second] 
                << ": " << t.first.second
                << std::endl;
    }
    for(auto t: D.tokens) {
        fout << "note right of ";
        fout << notations[t.first];
        fout << ": ";
        fout << t.second.Name;
        fout << std::endl;
    }
    fout.close();
}
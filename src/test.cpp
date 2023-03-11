#include <iostream>
#include <fstream>
#include "DFA.h"

int testing_multipleNFA2DFA() {
    std::stack<Expression> stk;

    Expression e{"ab"};
    e.in2post();
    e.ConstructNFA();
    stk.push(e);

    Expression d{"a|b"};
    d.in2post();
    d.ConstructNFA();
    stk.push(d);

    Expression summary_exp = mergeExpressions(stk);

    DFA D;
    D.alphabet = summary_exp.alphabet;
    D.NFA2DFA(summary_exp.nfa);
    D.MoveTokens(summary_exp.tokens);
    show_DFA(D);
    show_tokens(D);
    
    return 0;
}

int testing_DFAMinimizing() {
    std::stack<Expression> stk;
    Expression e1({"ab"});
    Expression e2({"a|b"});
    e1.in2post(); e2.in2post();
    e1.ConstructNFA(); e2.ConstructNFA();
    stk.push(e1); stk.push(e2);
    Expression e = mergeExpressions(stk);

    DFA D;
    D.alphabet = e.alphabet;
    D.NFA2DFA(e.nfa);
    D.MoveTokens(e.tokens);
    show_DFA(D);
    show_tokens(D);
    DFA M = DFAMinimize(D);
    show_DFA(M);
    show_tokens(M);
    return 0;
}

std::vector<std::function<int()>> testing_lists = {
    // testing_multipleNFA2DFA,
    testing_DFAMinimizing
};

int main() {
    int num_failed = 0;
    for (size_t i = 0; i < testing_lists.size(); i++)
    {
        std::cout << "Testing " << i + 1 << " of " << testing_lists.size() << ":\n";
        testing_lists[i]() == 0 ? (std::cout << "[PASS]\n") : (num_failed++, std::cout << "[FAIL]\n");
    }

    //summary
    std::cout << "\n[SUMMARY]\n";
    std::cout << testing_lists.size() << " in total; "
        << num_failed << "failed; " << testing_lists.size() - num_failed 
        << "passed; " << std::endl;
    return 0;
}
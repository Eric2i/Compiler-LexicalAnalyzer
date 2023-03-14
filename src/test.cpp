#include <iostream>
#include <fstream>
#include "DFA.h"

int testing_regex2MinimalDFA() {
    std::ifstream fin("test/input/tokens.txt");
    std::cerr << "Read from test/input/tokens.txt" << std::endl;
    std::stack<Expression> stk;

    int sequence = 0;
    std::string token_name, token_pattern;
    while(std::getline(fin, token_name, ' ')) {
        std::getline(fin, token_pattern);
        token_name = token_name.erase(token_name.size()-1);
        // std::cerr << "token_name:" << token_name << " token_pattern:" << token_pattern << std::endl;
        
        Expression exp({token_pattern, token_name});
        exp.ConstructNFA(sequence++);
        stk.push(exp);
    }

    Expression e = mergeExpressions(stk);
    DFA D = Expression2DFA(e);
    DFA M = DFAMinimize(D);
    // show_DFA(M);
    show_tokens(M);
    make_mermaid(M);
    std::cerr << "minimized DFA exported to test/output/mermaid.txt" << std::endl;
    return 0;
}


std::vector<std::function<int()>> testing_lists = {
    testing_regex2MinimalDFA,
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
    std::cout << testing_lists.size() << " in total; ";
        // << num_failed << "failed; " << testing_lists.size() - num_failed 
        // << "passed; " << std::endl;
    return 0;
}
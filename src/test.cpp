#include <iostream>
#include <fstream>
#include "NFA.h"
#include "Expression.h"

int testing_regex_simulation()
{
    std::ifstream fin("test/input/single.txt");
    std::ifstream expectations("test/output/single.txt");
    std::string regex;
    
    fin >> regex;
    Expression e{regex};
    e.in2post();
    e.ConstructNFA();

    #if defined(DEBUG)
    std::cerr << "Transition Table:" << std::endl;
    show_NFA(e.nfa);
    #endif

    std::string sample;
    std::string exp;
    while(fin >> sample) {
        bool matching_result = e.NFASimulator(sample);

        expectations >> exp;
        assert(bool(exp[0]-'0') == matching_result);

        #if defined(DEBUG)
        std::cerr << "regex:[" << regex << "] accepts string: [" << sample << "] ? ";
        std::cerr << (matching_result? "Accepted": "Rejected") << std::endl;
        #endif
    }

    fin.close();
    return 0;
}

int testing_regex2NFA()
{
    std::ifstream fin("test/input/regex.txt");
    std::string regex;
    while (fin >> regex)
    {
        Expression e{regex};
        e.in2post();
        #if defined(DEBUG)
        std::cerr << "================================================================" << std::endl;
        std::cerr << "After in2post: " << std::endl;
        std::cerr << e.postfix << '\n';
        #endif
        e.ConstructNFA();
        std::cerr << "================================================================" << std::endl;
        std::cerr << "REGEX: " << e.expression << std::endl;
        std::cerr << "Transition Table:" << std::endl;
        show_NFA(e.nfa);
    }
    fin.close();
    return 0;
}

std::vector<std::function<int()>> testing_lists = {
    testing_regex_simulation,
    testing_regex2NFA
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
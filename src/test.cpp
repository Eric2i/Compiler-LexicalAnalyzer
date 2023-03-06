#include <iostream>
#include <fstream>
#include "NFA.h"
#include "Expression.h"

int main()
{
    std::ifstream fin("test/input/single.txt");
    std::fstream sstream;
    std::string regex;
    // while (fin > regex)
    {
        fin >> regex;
        Expression e{regex};
        e.in2post();
        std::cerr << "================================================================" << std::endl;
        std::cerr << "After in2post: " << std::endl;
        std::cerr << e.postfix << '\n';
        std::cerr << "================================================================" << std::endl;
        e.ConstructNFA();

        std::cerr << "Transition Table:" << std::endl;
        show_NFA(e.nfa);

        std::string sample;
        while(fin >> sample) {
            std::cerr << "regex:[" << regex << "] accepts lexeme: [" << sample << "] ? ";
            std::cerr << (e.NFASimulator(sample)? "Accepted": "Rejected") << std::endl;
        }
    }
    return 0;
}
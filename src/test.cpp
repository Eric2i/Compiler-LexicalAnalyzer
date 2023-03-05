#include <iostream>
#include <fstream>
#include "NFA.h"
#include "Expression.h"

int main(int argc, char **argv)
{
    std::ifstream fin("test/input/regex.txt");
    std::string regex;
    while (fin >> regex)
    {
        Expression e{regex};
        e.in2post();
        std::cerr << "================================================================" << std::endl;
        std::cerr << "After in2post: " << std::endl;
        std::cerr << e.postfix << '\n';
        std::cerr << "================================================================" << std::endl;
        e.ConstructNFA();
        std::cerr << "Transition Table:" << std::endl;
        show_NFA(e.nfa);
        std::cerr << "================================================================" << std::endl;
    }
    return 0;
}
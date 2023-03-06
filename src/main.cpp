#include <iostream> 
#include "Expression.h"
int main(int argc, char *argv[]) {
    if(argc == 1) {
        std::cout << "Usage: " << argv[0] << " \"<regex>\" \"<string>\" " << std::endl;
        return -1;
    }

    for(int i = 1; i < argc; i+=2) {
        std::string regex = argv[i]; 
        std::string sample = argv[i+1]; 

        std::cerr << "regex: " << regex << std::endl;
        std::cerr << "sample: " << sample << std::endl;

        Expression e({regex}); // (n-1) - (2) = n-3
        std::cout << (e.Match(sample)? "Match": "Mismatch") << std::endl;
    }
    return 0;
}
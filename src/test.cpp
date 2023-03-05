#include <iostream>
#include "NFA.h"
#include "Expression.h"

int main() {
    // Expression e{"1+2*3"};
    // e.Tokenize();
    // e.transform();
    // e.show_postfix_tokens();

    NFA s = char2NFA('a');
    report_counter();
    
    NFA s_star = KleenClosure(s);
    report_counter();
    show_NFA(s_star);

    return 0;
}
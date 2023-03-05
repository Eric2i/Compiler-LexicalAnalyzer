#include "Expression.h"
#include <stack>
#include <cmath>
#include <iostream> 

// Transform the infix_expression to postfix expression
void Expression::in2post()
{
    std::stack<char> stk;
    // stk.push("(");
    // this->infix_tokens.push_back(")");
    for (char c: this->expression)
    {
        // std::string token = this->infix_tokens[i];
        // std::cerr << "At token: " << token << std::endl;
        // std::cerr << "operator number: " << stk.size() << 
        //             " operand number: " << stk.size() << std::endl;
        if(isalnum(c)) {
            this->postfix += c;
        }
        else if (c == '(')
        {
            stk.push('(');
        }
        else if (c == ')')
        {
            while(!stk.empty() && stk.top() != '(')
            {
                this->postfix += stk.top();stk.pop();
            } 
            stk.pop();
        }
        else {
            while(!stk.empty() && PartialOrd(stk.top(), c)) {
                this->postfix += stk.top();
                stk.pop();
            }
            stk.push(c);
        }
    }
    while(!stk.empty()) {
        this->postfix += stk.top();
        stk.pop();
    }
}

// Compare two operators
bool Expression::PartialOrd(const char a, const char b) {
    const std::string ordered_operator_lists = "|.*";
    return ordered_operator_lists.find(a) >= ordered_operator_lists.find(b);
}

void Expression::ConstructNFA() {
    std::stack<NFA> operands;
    reset_state_counter();
    for (char c : this->postfix) {
        if (isalnum(c)) {
            operands.push(char2NFA(c));
        } else {
            NFA nfa1 = operands.top();
            operands.pop();
            if (c == '*') {
                operands.push(KleenClosure(nfa1));
            } else {
                NFA nfa2 = operands.top();
                operands.pop();
                if (c == '.') {
                    operands.push(Concat(nfa2, nfa1));
                } else {
                    operands.push(Union(nfa2, nfa1));
                }
            }
        }
    }

    this->nfa = operands.top();
}
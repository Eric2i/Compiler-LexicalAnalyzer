#include "Expression.h"
#include <stack>
#include <cmath>
#include <iostream> 

// Transform the infix_expression to postfix expression
void Expression::in2post()
{
    std::stack<char> stk;
    char lastToken = '\0';
    for (char c: this->expression)
    {
        // std::cerr << "lastToken = " << lastToken << " currentToken = " << c << std::endl;
        if(isalnum(c)) {
            if(isalnum(lastToken) || lastToken=='*' || lastToken == ')') {
                char Concatenation = '#';
                while(!stk.empty() && PartialOrd(stk.top(), Concatenation)) {
                    this->postfix += stk.top();
                    stk.pop();
                }
                stk.push(Concatenation);
            }
            this->postfix += c;
            lastToken = c;
        }
        else if (c == '(')
        {
            if(isalnum(lastToken) || lastToken=='*' || lastToken == ')') {
                char Concatenation = '#';
                while(!stk.empty() && PartialOrd(stk.top(), Concatenation)) {
                    this->postfix += stk.top();
                    stk.pop();
                }
                stk.push(Concatenation);
            }
            stk.push('(');
            lastToken = c;
        }
        else if (c == ')')
        {
            while(!stk.empty() && stk.top() != '(')
            {
                this->postfix += stk.top();stk.pop();
            } 
            stk.pop();
            lastToken = ')';
        }
        else {
            while(!stk.empty() && PartialOrd(stk.top(), c)) {
                this->postfix += stk.top();
                stk.pop();
            }
            stk.push(c);
            lastToken = c;
        }

        // std::cerr << "stk.top() = " << (stk.empty()?'_':stk.top()) << std::endl;
        // std::cerr << "this->postfix = " << this->postfix << std::endl << std::endl;
    }
    while(!stk.empty()) {
        this->postfix += stk.top();
        stk.pop();
    }
}

// Compare two operators
bool Expression::PartialOrd(const char a, const char b) {
    const std::string ordered_operator_lists = "(|#+*?";
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
            } else if(c == '+') {
                operands.push(PositiveClosure(nfa1));
            } else if(c == '?') {
                operands.push(Union(nfa1, char2NFA('\0'))); // r? <=> r | epsilon
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
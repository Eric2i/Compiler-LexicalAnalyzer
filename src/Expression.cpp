#include "Expression.h"
#include <stack>
#include <cmath>
#include <iostream> 

const char EPSILON = '\0';

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

// build NFA from postfix regex
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
                if (c == '#') {
                    operands.push(Concat(nfa2, nfa1));
                } else {
                    operands.push(Union(nfa2, nfa1));
                }
            }
        }
    }

    this->nfa = operands.top();
}

std::set<int> Expression::epsilonClousure(int s) {
    std::set<int> states;
    states.insert(s);
    for(auto i: this->nfa.states[s].outEdges) {
        if(i.label == EPSILON) {
            states.insert(i.dest);
        }
    }
    return states;
}

std::stack<int> oldStates, newStates;
std::vector<bool> alreadyOn;

void Expression::addState(int s) {
    // std::cerr << "addState: " << s << std::endl;
    newStates.push(s);
    alreadyOn[s] = true;
    for(auto t: this->nfa.states[s].outEdges) {
        if(!alreadyOn[t.dest] && t.label == EPSILON) {
            addState(t.dest);
        }
    }
}

void report_currentStates() {
    std::cerr << "Report current states" << std::endl;
    std::stack<int> currentStates(oldStates);
    while(!currentStates.empty()) { 
        std::cerr << currentStates.top() << " ";
        currentStates.pop();
    }
    std::cerr << std::endl;
}

bool Expression::NFASimulator(const std::string & s) {
    alreadyOn.resize(report_counter());
    addState(this->nfa.start);
    while(!newStates.empty()) {
        oldStates.push(newStates.top());
        alreadyOn[newStates.top()] = false;
        newStates.pop();
    }
    // report_currentStates();
    for(auto c: s) {
        while(!oldStates.empty()) {
            for(auto t: this->nfa.states[oldStates.top()].outEdges) {
                if(t.label == c) addState(t.dest);
            }
            oldStates.pop();
        }
        while(!newStates.empty()) {
            oldStates.push(newStates.top());
            alreadyOn[newStates.top()] = false;
            newStates.pop();
        }
        // report_currentStates();
    }
    
    // final state in oldStates? return true: return false
    while(!oldStates.empty()) {
        if(oldStates.top() == this->nfa.accept) return true;
        oldStates.pop();
    }
    return false;
};

bool Expression::Match(const std::string & s) {
    this->in2post();
    this->ConstructNFA();
    return this->NFASimulator(s);
}
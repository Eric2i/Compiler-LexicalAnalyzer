#include "Expression.h"
#include <stack>
#include <cmath>
#include <iostream>

const char EPSILON = '\0';

std::map<std::string, std::string> var2regex; // regular definitions
void Expression::definition2regex() {
    // std::cerr << "before converting" << this->expression << std::endl;
    for(int i = 0; i < this->expression.size(); i++) { 
        if(this->expression[i] == '\\') {
            i += 1; // always skip the backslash
        }
        else if(this->expression[i] == '{') {
            int delta = 0;
            while(this->expression[i+delta] != '}') delta++;
            if(var2regex.find(this->expression.substr(i+1, delta-1)) == var2regex.end()) 
                {std::cerr << "TARGET {"<< this->expression.substr(i+1, delta-1) <<"} NOT FOUND!" << std::endl; exit(1);}
            this->expression.replace(i, delta+1, var2regex[this->expression.substr(i+1, delta-1)]);
            i += var2regex[this->expression.substr(i+1, delta-1)].size();
        }
    }
    // std::cerr << "after converting " << this->expression << std::endl;
    return;
}

bool isInAlphabet(char c) {
    if(isalnum(c)) return true;
    switch(c) {
        case '_':
            return true;
        case '<':
            return true;
        case '=':
            return true;
        case '!':
            return true;
        case '>':
            return true; 
        case '-':
            return true;
        case ' ':
            return true;
        case ';':
            return true;
    }
    return false;
}

bool allowConcatFollow(char c) {
    if(isInAlphabet(c)) return true;
    switch(c) {
        case '*':
            return true;
        case ')':
            return true;
        case '?':
            return true;
        case '\\':
            return true;
    }
    return false;
}


// Transform the infix_expression to postfix expression
void Expression::in2post()
{
    this->definition2regex();
    var2regex[this->token_name] = "(" + this->expression + ")";
    std::stack<char> stk;
    char lastToken = '\0';
    for (int i = 0; i < this->expression.size(); i++) 
    {
        char c = this->expression[i];
        if(c == '\\') {
            if(allowConcatFollow(lastToken)) {
                char Concatenation = '#';
                while(!stk.empty() && PartialOrd(stk.top(), Concatenation)) {
                    this->postfix += stk.top();
                    stk.pop();
                }
                stk.push(Concatenation);
            }
            char nextChar = this->expression[i+1];
            this->alphabet.insert(nextChar);
            this->postfix += c; this->postfix += nextChar;
            lastToken = c;
            i += 1; // skip the next character
        }
        else if(isInAlphabet(c)) {
            if(allowConcatFollow(lastToken)) {
                char Concatenation = '#';
                while(!stk.empty() && PartialOrd(stk.top(), Concatenation)) {
                    this->postfix += stk.top();
                    stk.pop();
                }
                stk.push(Concatenation);
            }
            this->alphabet.insert(c);
            this->postfix += c;
            lastToken = c;
        }
        else if (c == '(')
        {
            if(allowConcatFollow(lastToken)) {
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
        else { // regular expression operators
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
    const std::string ordered_operator_lists = "(|#*?";
    return ordered_operator_lists.find(a) >= ordered_operator_lists.find(b);
}

// build NFA from postfix regex
void Expression::ConstructNFA(int sequence) {
    this->in2post();
    // std::cerr << this->token_name << ": " << this->postfix << std::endl;
    std::stack<NFA> operands;
    for (int i = 0; i < this->postfix.size(); ++i) {
        char c = this->postfix[i];
        if(c == '\\') {
            // case: special operand, push to stack
            c = this->postfix[++i];
            operands.push(char2NFA(c));
        }
        else if (isInAlphabet(c)) {
            // case: operand, push to stack
            operands.push(char2NFA(c));
        } else {
            // case: operator, do regular expression operations
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
    this->tokens[this->nfa.accept] = Token({sequence, this->token_name, this->expression});
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
    this->ConstructNFA(0);
    return this->NFASimulator(s);
}
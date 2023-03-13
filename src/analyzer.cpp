#include <iostream>
#include <fstream>
#include "DFA.h"

DFA getMinimizedDFA() {
    // open regular expression file
    std::ifstream fin("test/input/tokens.txt");
    
    // read from file
    std::stack<Expression> stk;
    std::string token_name, token_pattern;
    int sequence = 0;
    while(std::getline(fin, token_name, ' ')) {
        std::getline(fin, token_pattern);
        token_name = token_name.erase(token_name.size()-1);
        // std::cerr << "token_name:" << token_name << " token_pattern:" << token_pattern << std::endl;
        Expression exp({token_pattern, token_name});
        exp.ConstructNFA(sequence++);
        stk.push(exp);
    }

    // merge multiple expressions (actually merging NFAs)
    Expression e = mergeExpressions(stk);
    DFA D = Expression2DFA(e); // Construct DFA
    DFA M = DFAMinimize(D);    // Minimize DFA
    // show_DFA(M);
    // show_tokens(M);
    make_mermaid(M);
    return M;
}

std::vector<std::pair<Token, std::string>> DFASimulator(DFA &M, std::string &input) {
    std::vector<std::pair<Token, std::string>> tokens;

    std::stack<DFAState> states;
    std::string lexeme_buffer;
    states.push(M.start);
    for(int i = 0; i < input.size(); i++) {
        DFAState state = states.top();
        char c = input[i];
        // std::cerr << "current char:" << c << std::endl;
        if(M.alphabet.find(c) == M.alphabet.end()) {
            std::cerr << "ERROR: character " << c << " not in alphabet" << std::endl;
            exit(1);
        }
        DFAState next_state = M.trans(state, c);
        if(next_state != DFAState()) {
            states.push(next_state);
            lexeme_buffer += c;
        } else {
            i -= 1;
            tokens.push_back({M.tokens[state], lexeme_buffer});
            while(!states.empty()) states.pop();
            lexeme_buffer = "";
            states.push(M.start);
        }
    }
    
    tokens.push_back({M.tokens[states.top()], lexeme_buffer});

    return tokens;
}

int main() {
    // get minimized DFA
    DFA M = getMinimizedDFA();
    // std::cerr << "DFA Construction Completed" << std::endl;
    // get input string
    std::string input;
    std::ifstream fin("test/input/sourceCode.txt");
    std::ofstream fout("test/output/tokenized.txt");
    while(std::getline(fin, input)) {
        // std::cerr << "length of the line: " << input.size() << std::endl;
        // std::cerr << "================================================" << std::endl;
        // std::cerr << input << std::endl;
         // compute token vector
        std::vector<std::pair<Token, std::string>> v = DFASimulator(M, input);
        for(auto t: v) {
            Token token = t.first;
            if(token.Name == "delim" || token.Name == "ws") continue;
            std::string lexeme = t.second;
            fout << "<";
            fout << token.Name << ": " << lexeme;
            fout << ">";
        }
        fout << std::endl;
        // std::cerr << "================================================" << std::endl;
    }
    return 0;
}
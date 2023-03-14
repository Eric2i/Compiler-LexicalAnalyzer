#include <iostream>
#include <fstream>
#include <cstring>
#include "DFA.h"

DFA getMinimizedDFA(char* filepath) {
    // open regular expression file
    std::ifstream fin(filepath);
    
    // read from file
    std::stack<Expression> stk;
    std::string token_name, token_pattern;
    int sequence = 0;
    while(std::getline(fin, token_name, ' ')) {
        std::getline(fin, token_pattern);
        token_name = token_name.erase(token_name.size()-1);
        std::cerr << "Processing Regular Expression:\n\ttoken_name:[" << token_name << "] \n\ttoken_pattern:[" << token_pattern << "]" << std::endl;
        Expression exp({token_pattern, token_name});
        exp.ConstructNFA(sequence++);
        stk.push(exp);
    }

    // merge multiple expressions (actually merging NFAs)
    Expression e = mergeExpressions(stk);
    std::cerr << "\nTransforming NFA to DFA..." << std::endl;
    DFA D = Expression2DFA(e); // Construct DFA
    std::cerr << "\nDFA Construction Completed!" << std::endl;
    return D;
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

int main(int argc, char **argv) {
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <tokens.txt> <sourceCode.txt> <tokenized.txt>" << std::endl;
        std::cerr << "<tokens.txt> contains a list of tokens, one per line." << std::endl;
        std::cerr << "<sourceCode.txt> contains codes to be tokenized." << std::endl;
        std::cerr << "<tokenized.txt> will store the tokenization output" << std::endl;
        exit(1);
    }
    else {
        // std::cerr << argv[1] << " " << argv[2] << " " << argv[3] << std::endl;
    }

    // get minimized DFA
    DFA M = getMinimizedDFA(argv[1]);
    std::cerr << "\nMinimized DFA Construction Completed!" << std::endl;
    std::cerr << "\nalphabet:\n"; for(auto i: M.alphabet) std::cerr << i << " "; std::cerr << std::endl;
    
    // tokenize source code
    std::string input;
    std::ifstream fin(argv[2]);
    std::ofstream fout(argv[3]);
    std::cerr << "\nReading source code..." << std::endl;
    while(std::getline(fin, input)) {
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
    }
    std::cerr << "\nTokenization Completed!" << std::endl;
    return 0;
}
#ifndef SCANNER_H
#define SCANNER_H

#include <memory>
#include <iostream>
#include "include/lexer/token.h"
#include "include/lexer/compiler.h"

namespace lexer {

    class Scanner {
    public:
        Scanner(const std::string &text, Compiler *compiler)
        : program(text), compiler(compiler), cur(&program) {}

        std::unique_ptr <Token> NextToken();

        void OutputComments() {
            for (auto comment : comments) {
                std::cout << comment << std::endl;
            }
        }

    private:
        std::string program;
        Compiler *compiler;
        std::vector <Fragment> comments;
        Position cur;
    };

}

#endif

#ifndef SCANNER_H
#define SCANNER_H

#include <memory>
#include "token.h"
#include "compiler.h"

class Scanner {
public:
    string Program;
    Scanner(const string& program, Compiler *compiler);
    unique_ptr<Token> NextToken();
private:
    Compiler *compiler;
    Position curr;
};

#endif

#ifndef INTERPRET_H
#define INTERPRET_H

#include "include/parser/node.h"

namespace semantics {
    class Interpreter {
    public:
        int Interpret(const parser::InnerNode& root);
    private:
        int ParseE(const parser::InnerNode& e);
        int ParseE1(const parser::InnerNode& e1);
        int ParseT(const parser::InnerNode& t);
        int ParseT1(const parser::InnerNode& t1);
        int ParseF(const parser::InnerNode& f);
    };
}

#endif

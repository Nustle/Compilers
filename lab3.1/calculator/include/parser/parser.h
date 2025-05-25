#ifndef PARSER_H
#define PARSER_H

#include "include/parser/node.h"
#include "include/lexer/scanner.h"
#include "include/parser/table.h"

namespace parser {

    class Parser {
    public:
        std::unique_ptr<Node> TopDownParse(lexer::Scanner *scanner);

    private:
        Table table = {
            #include "include/parser/table.h"
        };
    };
}

#endif

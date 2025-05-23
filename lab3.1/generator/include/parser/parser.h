#ifndef PARSER_H
#define PARSER_H

#include "include/parser/node.h"
#include "include/lexer/scanner.h"
#include "include/parser/analyzer_table.h"

namespace parser {

    class Parser {
    public:
        std::unique_ptr<Node> TopDownParse(lexer::Scanner *scanner);

    private:
        AnalyzerTable table = {
            #include "include/parser/analyzer_table.h"
        };
    };
}

#endif


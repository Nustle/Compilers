#include "include/parser/parser.h"
#include <sstream>

namespace parser {
    using StackItem = std::pair<Symbol, InnerNode*>;

    void ThrowParseError(const lexer::Token& token) {
        std::ostringstream err;
        err << token.GetCoords() << ": unexpected char " << lexer::ToStringToken(token);
        throw std::runtime_error(err.str());
    }

    std::unique_ptr<Node> Parser::TopDownParse(lexer::Scanner *scanner) {
        std::unique_ptr<InnerNode> dummy = std::make_unique<InnerNode>("dummy");
        auto stack = std::stack<StackItem>{};
        stack.emplace(EndOfProgram, dummy.get());
        stack.emplace(table.GetAxiom(), dummy.get());
        std::unique_ptr<lexer::Token> token = scanner->NextToken();

        do {
            auto& [symbol, parentPtr] = stack.top();
            InnerNode& parent = *parentPtr;
            if (symbol.GetType() == Symbol::Type::Terminal) {
                if (symbol.GetName() != lexer::ToStringTag(token->GetTag())) {
                    ThrowParseError(*token);
                }
                stack.pop();
                parent.AddChild(std::make_unique<LeafNode>(std::move(token)));
                token = scanner->NextToken();
            } else if (symbol.GetType() == Symbol::Type::NonTerminal) {
                const Symbol terminal = Symbol{lexer::ToStringTag(token->GetTag()), Symbol::Type::Terminal};
                OptSymbolVec rule_opt = table.Find(symbol, terminal);
                if (rule_opt) {
                    stack.pop();
                    InnerNode& child = dynamic_cast<InnerNode&>(parent.AddChild(std::make_unique<InnerNode>(symbol.GetName())));
                    SymbolVec rule = rule_opt->get();
                    for (int i = (int)rule.size() - 1; i >= 0; i--) {
                        stack.emplace(rule[i], &child);
                    }
                } else {
                    ThrowParseError(*token);
                }
            }

        } while (!stack.empty());

        return std::move(dummy->GetChildren().front());
    }

}
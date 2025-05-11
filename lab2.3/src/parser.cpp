#include "include/parser.h"
#include <sstream>

namespace parser {
    using StackItem = std::pair<Symbol, InnerNode*>;

    void ThrowParseError(lexer::Token token) {
        std::ostringstream err;
        err << "unexpected char in pos" << token.GetCoords();
        throw std::runtime_error(err.str());
    }

    std::unique_ptr<Node> Parser::TopDownParse(lexer::Scanner *scanner) {
        std::unique_ptr<InnerNode> dummy = std::make_unique<InnerNode>(NonTerminal::Dummy);
        auto stack = std::stack<StackItem>{};
        stack.push({{lexer::DomainTag::EndOfProgram}, dummy.get()});
        stack.push({{NonTerminal::Program}, dummy.get()});

        std::unique_ptr<lexer::Token> token = scanner->NextToken();

        do {
            auto& [symbol, parentPtr] = stack.top();
            InnerNode& parent = *parentPtr;
            if (auto* tag = std::get_if<lexer::DomainTag>(&symbol)) {
                if (token->GetTag() != *tag) {
                    ThrowParseError(*token);
                }
                stack.pop();
                parent.AddChild(std::make_unique<LeafNode>(std::move(token)));
                token = scanner->NextToken();
            } else {
                const NonTerminal non_terminal = std::get<NonTerminal>(symbol);

                SententialFormWrapped rule = table.Find(non_terminal, token->GetTag());
                if (rule) {
                    stack.pop();
                    InnerNode& child = dynamic_cast<InnerNode&>(parent.AddChild(std::make_unique<InnerNode>(non_terminal)));

                    SententialForm sf = rule->get();
                    std::vector<Symbol> cur_rules = sf.GetSententialForm();
                    for (int i = (int)cur_rules.size() - 1; i >= 0; i--) {
                        stack.push({cur_rules[i], &child});
                    }
                } else {
                    ThrowParseError(*token);
                }
            }
        } while (!stack.empty());

        return std::move(dummy->GetChildren().front());
    }

}

/*
Table table = {
#include "self-table.inc"
};
 */
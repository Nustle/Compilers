#ifndef NODE_H
#define NODE_H

#include <vector>
#include "token.h"

namespace parser {

    enum class NonTerminal {
        Program,
        Declaration,
        Rules,
        NonterminalDecl,
        Declaration1,
        Rule,
        Alternatives,
        Alternatives1,
        Alternative,
        Terms,
        Terms1,
        Term,
        Dummy,
    };

    inline std::ostream& operator<<(std::ostream& os, const NonTerminal &non_terminal) {
        switch (non_terminal) {
            case NonTerminal::Program: return os << "Program";
            case NonTerminal::Declaration: return os << "Declaration";
            case NonTerminal::Rules: return os << "Rules";
            case NonTerminal::NonterminalDecl: return os << "NonterminalDecl";
            case NonTerminal::Declaration1: return os << "Declaration1";
            case NonTerminal::Rule: return os << "Rule";
            case NonTerminal::Alternatives: return os << "Alternatives";
            case NonTerminal::Alternatives1: return os << "Alternatives1";
            case NonTerminal::Alternative: return os << "Alternative";
            case NonTerminal::Terms: return os << "Terms";
            case NonTerminal::Terms1: return os << "Terms1";
            case NonTerminal::Term: return os << "Term";
            case NonTerminal::Dummy: return os << "Dummy";
        }
    }

    class Node {
    public:
        virtual ~Node() = default;
        virtual void OutputTree(std::ostream& os) = 0;
    };

    class InnerNode : public Node {
    public:
        explicit InnerNode(NonTerminal non_terminal)
        : non_terminal(non_terminal) {}

        std::vector<std::unique_ptr<Node>>& GetChildren() {
            return children;
        }

        NonTerminal GetNonTerminal() const {
            return non_terminal;
        }

        Node& AddChild(std::unique_ptr<Node> &&node) {
            children.push_back(std::move(node));
            return *children.back();
        }
        void OutputTree(std::ostream& os) override;
    private:
        NonTerminal non_terminal;
        std::vector<std::unique_ptr<Node>> children;
    };

    class LeafNode : public Node {
    public:
        explicit LeafNode(std::unique_ptr<lexer::Token> &&token)
        : token(std::move(token)) {}

        std::unique_ptr<lexer::Token>& GetToken() {
            return token;
        }

        void OutputTree(std::ostream& os) override {}
    private:
        std::unique_ptr<lexer::Token> token;
    };

}

#endif


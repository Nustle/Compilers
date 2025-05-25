#ifndef NODE_H
#define NODE_H

#include <vector>
#include "include/lexer/token.h"

namespace parser {

    class Node {
    public:
        virtual ~Node() = default;
        virtual void OutputTree(std::ostream& os) = 0;
    };

    class InnerNode : public Node {
    public:
        explicit InnerNode(const std::string &non_terminal)
                : non_terminal(non_terminal) {}

        std::vector<std::unique_ptr<Node>>& GetChildren() {
            return children;
        }

        const std::vector<std::unique_ptr<Node>>& GetChildren() const {
            return children;
        }

        const std::string &GetNonTerminal() const {
            return non_terminal;
        }

        Node& AddChild(std::unique_ptr<Node> &&node) {
            children.push_back(std::move(node));
            return *children.back();
        }
        void OutputTree(std::ostream& os) override;
    private:
        std::string non_terminal;
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

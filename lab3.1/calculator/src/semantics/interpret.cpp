#include "include/semantics/interpret.h"

namespace semantics {
    int Interpreter::Interpret(const parser::InnerNode &root) {
        return ParseE(root);
    }

    // E ::= T E'
    int Interpreter::ParseE(const parser::InnerNode &e) {
        auto& child_ptr = e.GetChildren();
        const auto& t_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[0]);
        int t = ParseT(t_node);
        const auto& e1_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        int e1 = ParseE1(e1_node);
        return t + e1;
    }

    // T ::= F T'
    int Interpreter::ParseT(const parser::InnerNode& t) {
        auto& child_ptr = t.GetChildren();
        const auto& f_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[0]);
        int f = ParseF(f_node);
        const auto& t1_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        int t1 = ParseT1(t1_node);
        return f * t1;
    }

    // E' ::= + T E' | ε
    int Interpreter::ParseE1(const parser::InnerNode &e1) {
        auto& child_ptr = e1.GetChildren();
        if (child_ptr.empty()) {
            return 0;
        }
        const auto& t_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        int t = ParseT(t_node);
        const auto& e1_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[2]);
        int e1_new = ParseE1(e1_node);
        return e1_new + t;
    }

    // T' ::= * F T' | ε
    int Interpreter::ParseT1(const parser::InnerNode& t1) {
        auto& child_ptr = t1.GetChildren();
        if (child_ptr.empty()) {
            return 1;
        }
        const auto& f_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        int f = ParseF(f_node);
        const auto& t1_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[2]);
        int t1_new = ParseT1(t1_node);
        return f * t1_new;
    }

    // F  ::= n | ( E )
    int Interpreter::ParseF(const parser::InnerNode& f) {
        auto& child_ptr = f.GetChildren();
        if (child_ptr.size() == 1) {
            const auto& number_node = dynamic_cast<parser::LeafNode*>(child_ptr[0].get());
            const auto& token_from_node = *number_node->GetToken();
            auto* number_token = dynamic_cast<const lexer::NumberToken*>(&token_from_node);
            return number_token->GetVal();
        }
        const auto& e_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        int e = ParseE(e_node);
        return e;
    }
}

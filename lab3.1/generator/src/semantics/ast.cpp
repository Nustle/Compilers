#include "include/semantics/ast.h"
#include <unordered_set>

namespace semantics {

    // Program ::= Declaration Rules
    std::shared_ptr<Program> ConverterGrammar::ParseProgram(const parser::InnerNode& program) {
        axiom = nullptr;
        auto& child_ptr = program.GetChildren();
        const auto& decl_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[0]);
        SententialForm declaration = ParseDeclaration(decl_node);

        const auto& rules_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        std::vector<std::unique_ptr<Rule> > rules = ParseRules(rules_node);
        if (axiom == nullptr) {
            throw std::runtime_error("missing axiom");
        }
        return std::make_unique<Program>(std::move(*axiom), std::move(declaration), std::move(rules));
    }

    // Declaration ::= NonterminalDecl Declaration1
    SententialForm ConverterGrammar::ParseDeclaration(const parser::InnerNode& declaration) {
        auto& child_ptr = declaration.GetChildren();
        const auto& nonterminal_decl_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[0]);
        parser::Symbol nonterminal_decl = ParseNonterminalDecl(nonterminal_decl_node);

        const auto& declaration1_node = dynamic_cast<const parser::InnerNode&>(*child_ptr[1]);
        SententialForm declaration1 = ParseDeclaration1(declaration1_node);
        declaration1.push_back(nonterminal_decl);
        std::rotate(declaration1.rbegin(), declaration1.rbegin() + 1, declaration1.rend());
        return declaration1;
    }

    // Declaration1 ::= COMMA NonterminalDecl Declaration1 | ε
    SententialForm ConverterGrammar::ParseDeclaration1(const parser::InnerNode& declaration1) {
        const auto& children = declaration1.GetChildren();
        if (children.empty()) {
            return {};
        } else {
            const auto& nonterminal_decl_node = dynamic_cast<const parser::InnerNode&>(*children[1]);
            parser::Symbol nonterminal_decl = ParseNonterminalDecl(nonterminal_decl_node);
            const auto& declaration1_node = dynamic_cast<const parser::InnerNode&>(*children[2]);
            SententialForm decl1 = ParseDeclaration1(declaration1_node);
            decl1.push_back(nonterminal_decl);
            std::rotate(decl1.rbegin(), decl1.rbegin() + 1, decl1.rend());
            return decl1;
        }
    }

    // NonterminalDecl ::= NONTERMINAL | LEFT_BRACE NONTERMINAL RIGHT_BRACE
    parser::Symbol ConverterGrammar::ParseNonterminalDecl(const parser::InnerNode& nonterminal_decl) {
        const auto& children = nonterminal_decl.GetChildren();
        if (children.size() == 1) {
            const auto& nonterminal_node = dynamic_cast<parser::LeafNode*>(children[0].get());
            const auto& token_from_node = *nonterminal_node->GetToken();
            auto* nonterminal_token = dynamic_cast<const lexer::NonTerminalToken*>(&token_from_node);
            return {nonterminal_token->GetVal(), parser::Symbol::Type::NonTerminal};
        } else {
            if (axiom != nullptr) {
                throw std::runtime_error("axiom redefinition");
            }
            const auto& axiom_node = dynamic_cast<parser::LeafNode*>(children[1].get());
            const auto& axiom_from_node = *axiom_node->GetToken();
            auto* axiom_token = dynamic_cast<const lexer::NonTerminalToken*>(&axiom_from_node);
            parser::Symbol axiom_symbol = {axiom_token->GetVal(), parser::Symbol::Type::NonTerminal};
            axiom = std::make_unique<parser::Symbol>(axiom_symbol);
            return axiom_symbol;
        }
    }

    // Rules ::= Rule Rules | ε
    std::vector<std::unique_ptr<Rule>> ConverterGrammar::ParseRules(const parser::InnerNode& rules) {
        const auto& children = rules.GetChildren();
        if (children.empty()) {
            return {};
        } else {
            const auto& rule_node = dynamic_cast<const parser::InnerNode&>(*children[0]);
            std::unique_ptr<Rule> rule = ParseRule(rule_node);
            const auto& rules_node = dynamic_cast<const parser::InnerNode&>(*children[1]);
            std::vector<std::unique_ptr<Rule>> rls = ParseRules(rules_node);
            rls.push_back(std::move(rule));
            std::rotate(rls.rbegin(), rls.rbegin() + 1, rls.rend());
            return rls;
        }
    }

    // Rule ::= LEFT_ANGLE NONTERMINAL Alternatives RIGHT_ANGLE
    std::unique_ptr<Rule> ConverterGrammar::ParseRule(const parser::InnerNode& rule) {
        const auto& children = rule.GetChildren();
        const auto& lhs_node = dynamic_cast<parser::LeafNode*>(children[1].get());
        const auto& token_from_lhs = *lhs_node->GetToken();
        auto* lhs_token = dynamic_cast<const lexer::NonTerminalToken*>(&token_from_lhs);

        parser::Symbol lhs = {lhs_token->GetVal(), parser::Symbol::Type::NonTerminal};
        const auto& alternatives_node = dynamic_cast<const parser::InnerNode&>(*children[2]);
        auto alternatives = ParseAlternatives(alternatives_node);

        return std::make_unique<Rule>(std::move(lhs), std::move(alternatives));
    }

    // Alternatives ::= Alternative Alternatives1
    std::vector<std::unique_ptr<SententialForm>> ConverterGrammar::ParseAlternatives(const parser::InnerNode& alternatives) {
        const auto& children = alternatives.GetChildren();
        const auto& alternative_node = dynamic_cast<const parser::InnerNode&>(*children[0]);
        std::unique_ptr<SententialForm> alternative = ParseAlternative(alternative_node);
        const auto& alternatives1_node = dynamic_cast<const parser::InnerNode&>(*children[1]);
        auto alternatives1 = ParseAlternatives1(alternatives1_node);
        alternatives1.push_back(std::move(alternative));
        std::rotate(alternatives1.rbegin(), alternatives1.rbegin() + 1, alternatives1.rend());
        return alternatives1;
    }

    // Alternatives1 ::= Alternatives | ε
    std::vector<std::unique_ptr<SententialForm>> ConverterGrammar::ParseAlternatives1(const parser::InnerNode& alternatives1) {
        const auto& children = alternatives1.GetChildren();
        if (children.empty()) {
            return {};
        } else {
            const auto& alternatives_node = dynamic_cast<const parser::InnerNode&>(*children[0]);
            std::vector<std::unique_ptr<SententialForm>> alternatives = ParseAlternatives(alternatives_node);
            return alternatives;
        }
    }

    // Alternative ::= COLON Terms
    std::unique_ptr<SententialForm> ConverterGrammar::ParseAlternative(const parser::InnerNode& alternative) {
        const auto& children = alternative.GetChildren();
        const auto& terms_node = dynamic_cast<const parser::InnerNode&>(*children[1]);
        std::unique_ptr<SententialForm> terms = ParseTerms(terms_node);
        return terms;
    }

    // Terms ::= Term Terms1
    std::unique_ptr<SententialForm> ConverterGrammar::ParseTerms(const parser::InnerNode& terms) {
        const auto& children = terms.GetChildren();
        const auto& term_node = dynamic_cast<const parser::InnerNode&>(*children[0]);
        parser::Symbol term = ParseTerm(term_node);
        const auto& terms1_node = dynamic_cast<const parser::InnerNode&>(*children[1]);
        SententialForm terms1 = ParseTerms1(terms1_node);
        if (term.GetType() != parser::Symbol::Type::Spec) {
            terms1.push_back(term);
            std::rotate(terms1.rbegin(), terms1.rbegin() + 1, terms1.rend());
        }
        return std::make_unique<SententialForm>(terms1);
    }

    // Terms1 ::= Terms | ε
    SententialForm ConverterGrammar::ParseTerms1(const parser::InnerNode& terms1) {
        const auto& children = terms1.GetChildren();
        if (children.empty()) {
            return {};
        } else {
            const auto& terms_node = dynamic_cast<const parser::InnerNode&>(*children[0]);
            std::unique_ptr<SententialForm> terms = ParseTerms(terms_node);
            return *terms;
        }
    }

    // Term ::= TERMINAL | NONTERMINAL | EPS
    parser::Symbol ConverterGrammar::ParseTerm(const parser::InnerNode& term) {
        const auto& children = term.GetChildren();
        const auto& term_node = dynamic_cast<parser::LeafNode*>(children[0].get());
        const auto& token_from_node = *term_node->GetToken();
        switch (token_from_node.GetTag()) {
            case lexer::DomainTag::Terminal: {
                //auto* terminal_token = dynamic_cast<const lexer::TerminalToken*>(&token_from_node);
                return {lexer::ToStringToken(token_from_node), parser::Symbol::Type::Terminal};
            }
            case lexer::DomainTag::NonTerminal: {
                //auto* nonterminal_token = dynamic_cast<const lexer::NonTerminalToken*>(&token_from_node);
                return {lexer::ToStringToken(token_from_node), parser::Symbol::Type::NonTerminal};
            }
            case lexer::DomainTag::Eps: {
                //auto* eps_token = dynamic_cast<const lexer::SpecToken*>(&token_from_node);
                // TODO: когда пишем ToStringToken, то "@" и @ - одно и то же. Надо ToStringTag
                //return {lexer::ToStringToken(*eps_token), parser::Symbol::Type::Terminal};
                return parser::Epsilon;
            }
            default: {
                throw std::runtime_error("unknown term types");
            }
        }
    }

    void Program::CheckSemantics() {
        std::unordered_set<parser::Symbol> seen;
        for (auto &nt : non_terminals) {
            if (!seen.insert(nt).second) {
                throw std::runtime_error("Duplicate nonterminal: " + nt.GetName());
            }
        }
        std::unordered_set<parser::Symbol> lhs_set;
        for (auto &rp : rules) {
            lhs_set.insert(rp->GetLhs());
        }
        for (auto &nt : non_terminals) {
            if (lhs_set.count(nt) == 0) {
                throw std::runtime_error("Nonterminal never defined: " + nt.GetName());
            }
        }
        for (auto &rp : rules) {
            for (auto &prod_ptr : rp->GetRhs()) {
                for (auto &sym : *prod_ptr) {
                    if (sym.GetType() == parser::Symbol::Type::NonTerminal) {
                        if (lhs_set.count(sym) == 0) {
                            throw std::runtime_error("Useless symbol: " + sym.GetName());
                        }
                    }
                }
            }
        }
    }
}
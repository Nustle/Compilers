#ifndef AST_H
#define AST_H

#include "include/parser/symbol.h"
#include "include/parser/node.h"
#include <cassert>
#include <iostream>

namespace semantics {
    using SententialForm = std::vector<parser::Symbol>;

    class Rule {
    public:
        Rule(parser::Symbol&& lhs_in, std::vector<std::unique_ptr<SententialForm>>&& rhs_in)
        : lhs(std::move(lhs_in)) {
            rhs = std::move(rhs_in);
            assert(!rhs.empty());
        }
        const parser::Symbol& GetLhs() const {
            return lhs;
        }
        const std::vector<std::unique_ptr<SententialForm>>& GetRhs() const {
            return rhs;
        }
    private:
        parser::Symbol lhs;
        std::vector<std::unique_ptr<SententialForm> > rhs;
    };

    class Program {
    public:
        Program(parser::Symbol&& axiom_, std::vector<parser::Symbol>&& non_terminals_, std::vector<std::unique_ptr<Rule>>&& rules_)
        : axiom(std::move(axiom_)), non_terminals(non_terminals_), rules(std::move(rules_)) {
            CheckSemantics();
        }
        const parser::Symbol& GetAxiom() {
            return axiom;
        }

        SententialForm GetNonterminals() {
            return non_terminals;
        }

        const std::vector<std::unique_ptr<Rule>>& GetRules() const {
            return rules;
        }
    private:
        void CheckSemantics();
        parser::Symbol axiom;
        SententialForm non_terminals;
        std::vector<std::unique_ptr<Rule>> rules;
    };

    class ConverterGrammar {
    public:
        std::shared_ptr<Program> ParseProgram(const parser::InnerNode& program);
        SententialForm ParseDeclaration(const parser::InnerNode& declaration);
        SententialForm ParseDeclaration1(const parser::InnerNode& declaration1);
        parser::Symbol ParseNonterminalDecl(const parser::InnerNode& nonterminal_decl);
        std::vector<std::unique_ptr<Rule>> ParseRules(const parser::InnerNode& rules);
        std::unique_ptr<Rule> ParseRule(const parser::InnerNode& rule);
        std::vector<std::unique_ptr<SententialForm>> ParseAlternatives(const parser::InnerNode& alternatives);
        std::vector<std::unique_ptr<SententialForm>> ParseAlternatives1(const parser::InnerNode& alternatives1);
        std::unique_ptr<SententialForm> ParseAlternative(const parser::InnerNode& alternative);
        std::unique_ptr<SententialForm> ParseTerms(const parser::InnerNode& terms);
        SententialForm ParseTerms1(const parser::InnerNode& terms1);
        parser::Symbol ParseTerm(const parser::InnerNode& term);
    private:
        std::unique_ptr<parser::Symbol> axiom;
    };
}

#endif

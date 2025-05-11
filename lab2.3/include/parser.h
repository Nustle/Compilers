#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "scanner.h"

namespace parser {

    using Symbol = std::variant<NonTerminal, lexer::DomainTag>;

    class SententialForm {
    public:
        explicit SententialForm(std::vector<Symbol> right)
        : right (std::move(right)) {}

        std::vector<Symbol> GetSententialForm() {
            return right;
        }
    private:
        std::vector<Symbol> right;
    };

    using SententialFormWrapped = std::optional<std::reference_wrapper<const SententialForm>>;

    class AnalyzerTable {
    public:
        AnalyzerTable()
        : rules({
            SententialForm({NonTerminal::Declaration, NonTerminal::Rules}), /* 0 */
            SententialForm({NonTerminal::NonterminalDecl, NonTerminal::Declaration1}), /* 1  */
            SententialForm({lexer::DomainTag::Comma, NonTerminal::NonterminalDecl, NonTerminal::Declaration1}), /* 2 */
            SententialForm({}), /* 3 */
            SententialForm({lexer::DomainTag::NonTerminal}), /* 4 */
            SententialForm({lexer::DomainTag::LeftBrace, lexer::DomainTag::NonTerminal, lexer::DomainTag::RightBrace}), /* 5 */
            SententialForm({NonTerminal::Rule, NonTerminal::Rules}), /* 6 */
            SententialForm({lexer::DomainTag::LeftAngle, lexer::DomainTag::NonTerminal, NonTerminal::Alternatives, lexer::DomainTag::RightAngle}), /* 7 */
            SententialForm({NonTerminal::Alternative, NonTerminal::Alternatives1}), /* 8 */
            SententialForm({NonTerminal::Alternatives}), /* 9 */
            SententialForm({lexer::DomainTag::Colon, NonTerminal::Terms}), /* 10 */
            SententialForm({NonTerminal::Term, NonTerminal::Terms1}), /* 11 */
            SententialForm({NonTerminal::Terms}), /* 12 */
            SententialForm({lexer::DomainTag::Terminal}), /* 13 */
            SententialForm({lexer::DomainTag::Eps}), /* 14 */
        }), vals({
              {NonTerminal::Program,
               {{{lexer::DomainTag::NonTerminal}, rules[0]}, {{lexer::DomainTag::LeftBrace}, rules[0]}}
              },
              {NonTerminal::Declaration,
               {{{lexer::DomainTag::NonTerminal}, rules[1]}, {{lexer::DomainTag::LeftBrace}, rules[1]}}
              },
              {NonTerminal::Rules,
               {{{lexer::DomainTag::LeftAngle}, rules[6]}, {{lexer::DomainTag::EndOfProgram}, rules[3]}}
              },
              {NonTerminal::NonterminalDecl,
               {{{lexer::DomainTag::NonTerminal}, rules[4]}, {{lexer::DomainTag::LeftBrace}, rules[5]}}
              },
              {NonTerminal::Declaration1,
               {{{lexer::DomainTag::LeftAngle}, rules[3]}, {{lexer::DomainTag::Comma}, rules[2]}, {{lexer::DomainTag::EndOfProgram}, rules[3]}}
              },
              {NonTerminal::Rule,
               {{{lexer::DomainTag::LeftAngle}, rules[7]}}
              },
              {NonTerminal::Alternatives,
               {{{lexer::DomainTag::Colon}, rules[8]}}
              },
              {NonTerminal::Alternatives1,
               {{{lexer::DomainTag::Colon}, rules[9]}, {{lexer::DomainTag::RightAngle}, rules[3]}}
              },
              {NonTerminal::Alternative,
               {{{lexer::DomainTag::Colon}, rules[10]}}
              },
              {NonTerminal::Terms,
               {{{lexer::DomainTag::NonTerminal}, rules[11]}, {{lexer::DomainTag::Terminal}, rules[11]}, {{lexer::DomainTag::Eps}, rules[11]}}
              },
              {NonTerminal::Terms1,
               {{{lexer::DomainTag::NonTerminal}, rules[12]}, {{lexer::DomainTag::Terminal}, rules[12]}, {{lexer::DomainTag::Colon}, rules[3]}, {{lexer::DomainTag::RightAngle}, rules[3]}, {{lexer::DomainTag::Eps}, rules[12]}}
              },
              {NonTerminal::Term,
               {{{lexer::DomainTag::NonTerminal}, rules[4]}, {{lexer::DomainTag::Terminal}, rules[13]}, {{lexer::DomainTag::Eps}, rules[14]}}
              }
          }) {}

        SententialFormWrapped Find(NonTerminal non_terminal, lexer::DomainTag tag) {
            auto row_it = vals.find(non_terminal);
            if (row_it == vals.end())
                return std::nullopt;

            auto col_it = row_it->second.find(tag);
            if (col_it == row_it->second.end())
                return std::nullopt;

            return col_it->second;
        }
    private:
        std::vector<SententialForm> rules;
        std::unordered_map<NonTerminal, std::unordered_map<lexer::DomainTag, SententialForm> > vals;
    };

    class Parser {
    public:
        std::unique_ptr<Node> TopDownParse(lexer::Scanner *scanner);

    private:
        AnalyzerTable table;
    };
}

#endif


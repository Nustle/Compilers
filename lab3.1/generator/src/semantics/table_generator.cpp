#include "include/semantics/table_generator.h"
#include <fstream>

namespace semantics {
    void FirstFollow::BuildFirst(const std::shared_ptr<semantics::Program>& program) {
        for (const auto& nt : program->GetNonterminals()) {
            first[nt] = {};
        }

        bool changed;
        do {
            changed = false;
            for (const auto& rule : program->GetRules()) {
                auto left = rule->GetLhs();
                for (const auto& alt_ptr : rule->GetRhs()) {
                    const auto& alpha = *alt_ptr;
                    bool all_nullable = true;
                    for (const auto & sym_alpha : alpha) {
                        if (sym_alpha.GetType() == parser::Symbol::Type::Terminal) {
                            if (first[left].insert(sym_alpha).second) {
                                changed = true;
                            }
                            all_nullable = false;
                            break;
                        }
                        for (auto& t : first[sym_alpha]) {
                            if (!(t == parser::Epsilon)) {
                                if (first[left].insert(t).second) {
                                    changed = true;
                                }
                            }
                        }
                        if (!first[sym_alpha].count(parser::Epsilon)) {
                            all_nullable = false;
                            break;
                        }
                    }
                    if (all_nullable) {
                        if (first[left].insert(parser::Epsilon).second) {
                            changed = true;
                        }
                    }
                }
            }

        } while (changed);
    }

    void FirstFollow::BuildFollow(const std::shared_ptr<semantics::Program>& program) {
        for (auto const& left : program->GetNonterminals()) {
            follow[left] = {};
        }
        follow[program->GetAxiom()].insert(parser::EndOfProgram);

        bool changed;
        do {
            changed = false;
            for (auto const& rule_ptr : program->GetRules()) {
                const auto& left = rule_ptr->GetLhs();
                for (auto const& alt_ptr : rule_ptr->GetRhs()) {
                    const auto& alpha = *alt_ptr;
                    for (size_t i = 0; i < alpha.size(); ++i) {
                        const auto& sym_alpha = alpha[i];
                        if (sym_alpha.GetType() != parser::Symbol::Type::NonTerminal) {
                            continue;
                        }
                        bool all_nullable = true;
                        for (size_t j = i + 1; j < alpha.size(); ++j) {
                            const auto& sym_alpha_next = alpha[j];
                            if (sym_alpha_next.GetType() == parser::Symbol::Type::Terminal) {
                                if (follow[sym_alpha].insert(sym_alpha_next).second) {
                                    changed = true;
                                }
                                all_nullable = false;
                                break;
                            }
                            for (auto const& t : first[sym_alpha_next]) {
                                if (!(t == parser::Epsilon)) {
                                    if (follow[sym_alpha].insert(t).second) {
                                        changed = true;
                                    }
                                }
                            }
                            if (!first[sym_alpha_next].count(parser::Epsilon)) {
                                all_nullable = false;
                                break;
                            }
                        }
                        if (all_nullable) {
                            for (auto const& t : follow[left]) {
                                if (follow[sym_alpha].insert(t).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        } while (changed);
    }

    TableGenerator::TableGenerator(const std::shared_ptr<semantics::Program>& program_, const semantics::FirstFollow &first_follow)
    : program(program_) {
        auto first  = first_follow.GetFirst();
        auto follow = first_follow.GetFollow();

        for (auto const& rule_ptr : program->GetRules()) {
            const parser::Symbol left = rule_ptr->GetLhs();

            for (auto const& alt_ptr : rule_ptr->GetRhs()) {
                const SententialForm& alpha = *alt_ptr;
                std::unordered_set<parser::Symbol> first_alpha;
                bool all_nullable = true;
                for (auto const& alpha_sym : alpha) {
                    if (alpha_sym.GetType() == parser::Symbol::Type::Terminal) {
                        first_alpha.insert(alpha_sym);
                        all_nullable = false;
                        break;
                    }
                    auto const& first_alpha_sym = first[alpha_sym];
                    for (auto const& t : first_alpha_sym) {
                        if (!(t == parser::Epsilon)) {
                            first_alpha.insert(t);
                        }
                    }
                    if (!first_alpha_sym.count(parser::Epsilon)) {
                        all_nullable = false;
                        break;
                    }
                }
                if (all_nullable) {
                    first_alpha.insert(parser::Epsilon);
                }

                for (auto const& t : first_alpha) {
                    if (!(t == parser::Epsilon)) {
                        auto& row = table[left];
                        if (!row.emplace(t, alpha).second) {
                            throw std::runtime_error("Grammar not LL(1)");
                        }
                    }
                }
                
                if (first_alpha.count(parser::Epsilon)) {
                    for (auto const& t : follow[left]) {
                        auto& row = table[left];
                        if (!row.emplace(t, alpha).second) {
                            throw std::runtime_error("Grammar not LL(1)");
                        }
                    }
                }
            }
        }
    }

    void TableGenerator::Generate(const std::string& out_path) const {
        std::ofstream ofs(out_path);

        ofs << "#include \"include/parser/table.h\"\n\n";
        ofs << "namespace parser {\n\n";
        ofs << "Table::Table()\n"
               ": axiom({\"" << GetAxiom().GetName() << "\", Symbol::Type::"
            << (GetAxiom().GetType() == parser::Symbol::Type::NonTerminal ? "NonTerminal" : "Terminal")
            << "}),\n  table({\n";

        bool firstRow = true;
        for (auto const& [nt, row] : GetTable()) {
            if (!firstRow) ofs << ",\n";
            firstRow = false;

            ofs << "    {{\"" << nt.GetName() << "\", Symbol::Type::NonTerminal}, {\n";

            bool firstCol = true;
            for (auto const& [tk, alpha] : row) {
                if (!firstCol) ofs << ",\n";
                firstCol = false;

                ofs << "         {{\"" << tk.GetName() << "\", Symbol::Type::Terminal}, {";
                for (size_t i = 0; i < alpha.size(); ++i) {
                    if (i) ofs << ", ";
                    ofs << "{\"" << alpha[i].GetName() << "\", Symbol::Type::"
                        << (alpha[i].GetType() == parser::Symbol::Type::NonTerminal ? "NonTerminal" : "Terminal")
                        << "}";
                }
                ofs << "}}";
            }

            ofs << "\n    }}";
        }

        ofs << "\n  }) {}\n\n}";
    }

}
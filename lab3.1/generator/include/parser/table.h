#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <unordered_map>

#include "symbol.h"

namespace parser {

    class AnalyzerTable {
        Symbol axiom;
        std::unordered_map<Symbol, std::unordered_map<Symbol, std::vector<Symbol>> > table;

    public:
        AnalyzerTable();

        const Symbol& GetAxiom() const {
            return axiom;
        }

        OptSymbolVec Find(const Symbol& non_terminal, const Symbol& terminal) const {
            auto rowIt = table.find(non_terminal);
            if (rowIt == table.cend()) {
                return std::nullopt;
            }
            const auto& row = rowIt->second;
            auto colIt = row.find(terminal);
            if (colIt == row.cend()) {
                return std::nullopt;
            }
            return std::cref(colIt->second);
        }
    };
}

#endif

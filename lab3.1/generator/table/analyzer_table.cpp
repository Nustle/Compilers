#include "include/parser/analyzer_table.h"

namespace parser {

AnalyzerTable::AnalyzerTable()
: axiom({"E", Symbol::Type::NonTerminal}),
  table({
    {{"T", Symbol::Type::NonTerminal}, {
         {{"n", Symbol::Type::Terminal}, {{"F", Symbol::Type::NonTerminal}, {"T'", Symbol::Type::NonTerminal}}},
         {{"(", Symbol::Type::Terminal}, {{"F", Symbol::Type::NonTerminal}, {"T'", Symbol::Type::NonTerminal}}}
    }},
    {{"F", Symbol::Type::NonTerminal}, {
         {{"(", Symbol::Type::Terminal}, {{"(", Symbol::Type::Terminal}, {"E", Symbol::Type::NonTerminal}, {")", Symbol::Type::Terminal}}},
         {{"n", Symbol::Type::Terminal}, {{"n", Symbol::Type::Terminal}}}
    }},
    {{"E'", Symbol::Type::NonTerminal}, {
         {{")", Symbol::Type::Terminal}, {}},
         {{"EOF", Symbol::Type::Terminal}, {}},
         {{"+", Symbol::Type::Terminal}, {{"+", Symbol::Type::Terminal}, {"T", Symbol::Type::NonTerminal}, {"E'", Symbol::Type::NonTerminal}}}
    }},
    {{"T'", Symbol::Type::NonTerminal}, {
         {{")", Symbol::Type::Terminal}, {}},
         {{"+", Symbol::Type::Terminal}, {}},
         {{"EOF", Symbol::Type::Terminal}, {}},
         {{"*", Symbol::Type::Terminal}, {{"*", Symbol::Type::Terminal}, {"F", Symbol::Type::NonTerminal}, {"T'", Symbol::Type::NonTerminal}}}
    }},
    {{"E", Symbol::Type::NonTerminal}, {
         {{"(", Symbol::Type::Terminal}, {{"T", Symbol::Type::NonTerminal}, {"E'", Symbol::Type::NonTerminal}}},
         {{"n", Symbol::Type::Terminal}, {{"T", Symbol::Type::NonTerminal}, {"E'", Symbol::Type::NonTerminal}}}
    }}
  }) {}

}
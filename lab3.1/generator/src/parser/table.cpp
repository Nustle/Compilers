#include "include/parser/table.h"

namespace parser {

Table::Table()
: axiom({"Program", Symbol::Type::NonTerminal}),
  table({
    {{"Term", Symbol::Type::NonTerminal}, {
         {{"Eps", Symbol::Type::Terminal}, {{"Eps", Symbol::Type::Terminal}}},
         {{"NonTerminal", Symbol::Type::Terminal}, {{"NonTerminal", Symbol::Type::Terminal}}},
         {{"Terminal", Symbol::Type::Terminal}, {{"Terminal", Symbol::Type::Terminal}}}
    }},
    {{"Terms1", Symbol::Type::NonTerminal}, {
         {{"RightAngle", Symbol::Type::Terminal}, {}},
         {{"Colon", Symbol::Type::Terminal}, {}},
         {{"Eps", Symbol::Type::Terminal}, {{"Terms", Symbol::Type::NonTerminal}}},
         {{"NonTerminal", Symbol::Type::Terminal}, {{"Terms", Symbol::Type::NonTerminal}}},
         {{"Terminal", Symbol::Type::Terminal}, {{"Terms", Symbol::Type::NonTerminal}}}
    }},
    {{"Terms", Symbol::Type::NonTerminal}, {
         {{"Terminal", Symbol::Type::Terminal}, {{"Term", Symbol::Type::NonTerminal}, {"Terms1", Symbol::Type::NonTerminal}}},
         {{"NonTerminal", Symbol::Type::Terminal}, {{"Term", Symbol::Type::NonTerminal}, {"Terms1", Symbol::Type::NonTerminal}}},
         {{"Eps", Symbol::Type::Terminal}, {{"Term", Symbol::Type::NonTerminal}, {"Terms1", Symbol::Type::NonTerminal}}}
    }},
    {{"Alternative", Symbol::Type::NonTerminal}, {
         {{"Colon", Symbol::Type::Terminal}, {{"Colon", Symbol::Type::Terminal}, {"Terms", Symbol::Type::NonTerminal}}}
    }},
    {{"Rule", Symbol::Type::NonTerminal}, {
         {{"LeftAngle", Symbol::Type::Terminal}, {{"LeftAngle", Symbol::Type::Terminal}, {"NonTerminal", Symbol::Type::Terminal}, {"Alternatives", Symbol::Type::NonTerminal}, {"RightAngle", Symbol::Type::Terminal}}}
    }},
    {{"Program", Symbol::Type::NonTerminal}, {
         {{"LeftBrace", Symbol::Type::Terminal}, {{"Declaration", Symbol::Type::NonTerminal}, {"Rules", Symbol::Type::NonTerminal}}},
         {{"NonTerminal", Symbol::Type::Terminal}, {{"Declaration", Symbol::Type::NonTerminal}, {"Rules", Symbol::Type::NonTerminal}}}
    }},
    {{"NonterminalDecl", Symbol::Type::NonTerminal}, {
         {{"LeftBrace", Symbol::Type::Terminal}, {{"LeftBrace", Symbol::Type::Terminal}, {"NonTerminal", Symbol::Type::Terminal}, {"RightBrace", Symbol::Type::Terminal}}},
         {{"NonTerminal", Symbol::Type::Terminal}, {{"NonTerminal", Symbol::Type::Terminal}}}
    }},
    {{"Declaration", Symbol::Type::NonTerminal}, {
         {{"NonTerminal", Symbol::Type::Terminal}, {{"NonterminalDecl", Symbol::Type::NonTerminal}, {"Declaration1", Symbol::Type::NonTerminal}}},
         {{"LeftBrace", Symbol::Type::Terminal}, {{"NonterminalDecl", Symbol::Type::NonTerminal}, {"Declaration1", Symbol::Type::NonTerminal}}}
    }},
    {{"Declaration1", Symbol::Type::NonTerminal}, {
         {{"LeftAngle", Symbol::Type::Terminal}, {}},
         {{"EOF", Symbol::Type::Terminal}, {}},
         {{"Comma", Symbol::Type::Terminal}, {{"Comma", Symbol::Type::Terminal}, {"NonterminalDecl", Symbol::Type::NonTerminal}, {"Declaration1", Symbol::Type::NonTerminal}}}
    }},
    {{"Rules", Symbol::Type::NonTerminal}, {
         {{"EOF", Symbol::Type::Terminal}, {}},
         {{"LeftAngle", Symbol::Type::Terminal}, {{"Rule", Symbol::Type::NonTerminal}, {"Rules", Symbol::Type::NonTerminal}}}
    }},
    {{"Alternatives1", Symbol::Type::NonTerminal}, {
         {{"RightAngle", Symbol::Type::Terminal}, {}},
         {{"Colon", Symbol::Type::Terminal}, {{"Alternatives", Symbol::Type::NonTerminal}}}
    }},
    {{"Alternatives", Symbol::Type::NonTerminal}, {
         {{"Colon", Symbol::Type::Terminal}, {{"Alternative", Symbol::Type::NonTerminal}, {"Alternatives1", Symbol::Type::NonTerminal}}}
    }}
  }) {}

}
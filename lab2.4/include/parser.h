#ifndef PARSER_H
#define PARSER_H

#include "node.h"

namespace parser {
    class Parser final {
    public:
        Parser(std::unique_ptr<lexer::Scanner>&& scanner)
        : scanner(std::move(scanner)) {}

        Parser(const Parser& other) = delete;
        Parser& operator=(const Parser& other) = delete;

        std::unique_ptr<parser::Program> RecursiveDescentParse();

    private:
        std::unique_ptr<parser::Program> Program();
        std::unique_ptr<parser::Function> Function();
        std::vector<std::unique_ptr<parser::Stmt>> Statements();
        std::vector<std::unique_ptr<parser::Expr>> Params();
        std::unique_ptr<parser::Var> VarDef();
        std::unique_ptr<parser::Expr> Expr();
        std::unique_ptr<parser::Stmt> Statement();
        std::unique_ptr<parser::AssignStmt> AssignStmt();
        std::unique_ptr<parser::IfStmt> IfStmt();
        std::unique_ptr<parser::WhileStmt> WhileStmt();
        std::unique_ptr<parser::ForStmt> ForStmt();
        std::unique_ptr<parser::DimStmt> DimStmt();
        std::unique_ptr<parser::Expr> ArithmExpr();
        std::unique_ptr<parser::Expr> Term();
        std::unique_ptr<parser::Expr> Factor();
        std::unique_ptr<parser::Expr> Const();

        template <typename T>
        std::unique_ptr<T> ExpectAndCast(const lexer::DomainTag tag);

        void Expect(const lexer::DomainTag tag);

        template <typename T>
        std::unique_ptr<T> SymTo();

        [[noreturn]] void ThrowParseError(std::vector<lexer::DomainTag>&& expected);

        std::unique_ptr<lexer::Scanner> scanner;
        std::unique_ptr<lexer::Token> sym;
    };

}

#endif
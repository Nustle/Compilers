#include "include/parser.h"
#include <sstream>

namespace parser {
    using lexer::DomainTag;

    template <typename T>
    std::unique_ptr<T> Parser::ExpectAndCast(const DomainTag tag) {
        if (sym->GetTag() != tag) {
            ThrowParseError({tag});
        }
        auto ptr = SymTo<T>();
        sym = scanner->NextToken();
        return ptr;
    }

    void Parser::Expect(const DomainTag tag) {
        if (sym->GetTag() != tag) {
            ThrowParseError({tag});
        }
        sym = scanner->NextToken();
    }

    template <typename T>
    std::unique_ptr<T> Parser::SymTo() {
        return std::unique_ptr<T>{static_cast<T*>(sym.release())};
    }

    void Parser::ThrowParseError(std::vector<DomainTag>&& exp) {
        std::ostringstream oss;
        oss << sym->GetCoords() << ": expected ";
        for (auto t : exp) {
            oss << t << " ";
        }
        oss << "but got " << sym->GetTag();
        throw std::runtime_error(oss.str());
    }

    std::unique_ptr<Program> Parser::RecursiveDescentParse() {
        sym = scanner->NextToken();
        auto prog = Program();
        Expect(DomainTag::EndOfProgram);
        return prog;
    }

    // Program ::= Function* Statements
    std::unique_ptr<Program> Parser::Program() {
        std::vector<std::unique_ptr<parser::Function>> funs;
        while (sym->GetTag() == DomainTag::KFunction || sym->GetTag() == DomainTag::KSub) {
            funs.push_back(Function());
        }
        auto sts = Statements();
        return std::make_unique<parser::Program>(std::move(funs), std::move(sts));
    }

    //Function ::= 'Function' VarDef '(' Params? ')' Statements 'End' 'Function'
    //          |  'Sub' IDENT '(' Params? ')' Statements 'End' 'Sub'
    std::unique_ptr<parser::Function> Parser::Function() {
        bool is_sub = false;
        std::string func_name;
        std::string type_mark;

        if (sym->GetTag() == lexer::DomainTag::KFunction) {
            Expect(lexer::DomainTag::KFunction);
            auto var_def = VarDef();
            func_name = var_def->name;
            type_mark  = var_def->type;
        } else if (sym->GetTag() == lexer::DomainTag::KSub) {
            Expect(lexer::DomainTag::KSub);
            is_sub = true;
            auto ident_tok = ExpectAndCast<lexer::IdentToken>(lexer::DomainTag::Ident);
            func_name = ident_tok->GetVal();
            type_mark  = "";
        }

        Expect(lexer::DomainTag::LeftParen);
        std::vector<std::unique_ptr<parser::Expr>> params;
        if (sym->GetTag() == lexer::DomainTag::Plus || sym->GetTag() == lexer::DomainTag::Minus || sym->GetTag() == lexer::DomainTag::Ident) {
            params = Params();
        }
        Expect(lexer::DomainTag::RightParen);

        auto body = Statements();

        Expect(lexer::DomainTag::KEnd);
        Expect(is_sub ? lexer::DomainTag::KSub : lexer::DomainTag::KFunction);

        return std::make_unique<parser::Function>(
                is_sub,
                std::move(func_name),
                std::move(type_mark),
                std::move(params),
                std::move(body)
        );
    }

    // Params ::= Expr (',' Expr)*
    std::vector<std::unique_ptr<Expr>> Parser::Params() {
        std::vector<std::unique_ptr<parser::Expr>> params;
        params.push_back(Expr());
        while (sym->GetTag() == DomainTag::Comma) {
            sym = scanner->NextToken();
            params.push_back(Expr());
        }
        return params;
    }

    //VarDef ::= IDENT Type
    std::unique_ptr<Var> Parser::VarDef() {
        auto ident = ExpectAndCast<lexer::IdentToken>(DomainTag::Ident);
        auto type = ExpectAndCast<lexer::SpecToken>(DomainTag::Type);
        return std::make_unique<Var>(ident->GetVal(), type->GetVal());
    }

    // Statements ::= Statement*
    std::vector<std::unique_ptr<Stmt>> Parser::Statements() {
        std::vector<std::unique_ptr<Stmt>> stms;
        while (true) {
            switch (sym->GetTag()) {
                case DomainTag::Ident:
                case DomainTag::KIf:
                case DomainTag::KDo:
                case DomainTag::KFor:
                case DomainTag::KDim:
                    stms.push_back(Statement());
                    break;
                default:
                    return stms;
            }
        }
    }

    // Statement ::= AssignStmt | IfStmt | WhileStmt | ForStmt | DimStmt
    std::unique_ptr<Stmt> Parser::Statement() {
        switch (sym->GetTag()) {
            case DomainTag::Ident: {
                return AssignStmt();
            }
            case DomainTag::KIf: {
                return IfStmt();
            }
            case DomainTag::KDo: {
                return WhileStmt();
            }
            case DomainTag::KFor: {
                return ForStmt();
            }
            case DomainTag::KDim: {
                return DimStmt();
            }
            default: {
                ThrowParseError(
                    {DomainTag::Ident, DomainTag::KIf,
                         DomainTag::KDo, DomainTag::KFor, DomainTag::KDim}
                );
            }
        }
    }

    // AssignStmt ::= Factor '=' Expr
    std::unique_ptr<AssignStmt> Parser::AssignStmt() {
        auto lhs = Factor();
        Expect(DomainTag::Assign);
        auto rhs = Expr();
        return std::make_unique<parser::AssignStmt>(std::move(lhs), std::move(rhs));
    }

    // IfStmt ::= 'If' Expr 'Then' Statements ('Else' Statements)? 'End' 'If'
    std::unique_ptr<IfStmt> Parser::IfStmt() {
        Expect(DomainTag::KIf);
        auto cond = Expr();
        Expect(DomainTag::KThen);
        auto then_part = Statements();
        std::vector<std::unique_ptr<Stmt>> else_part;
        if (sym->GetTag() == DomainTag::KElse) {
            sym = scanner->NextToken();
            else_part = Statements();
        }
        Expect(DomainTag::KEnd);
        Expect(DomainTag::KIf);

        return std::make_unique<parser::IfStmt>(
                std::move(cond),
      std::move(then_part),
      std::move(else_part)
        );
    }

    // WhileStmt ::= 'Do' ( 'While' Expr  Statements 'Loop'
    //                    | 'Until' Expr  Statements 'Loop'
    //                    |  Statements 'Loop' 'While' Expr
    //                    |  Statements 'Loop' 'Until' Expr
    //                    |  Statements 'Loop' )
    std::unique_ptr<WhileStmt> Parser::WhileStmt() {
        Expect(DomainTag::KDo);

        if (sym->GetTag() == DomainTag::KWhile || sym->GetTag() == DomainTag::KUntil) {
            bool is_until = (sym->GetTag() == DomainTag::KUntil);
            sym = scanner->NextToken();
            auto cond = Expr();
            auto body = Statements();
            Expect(DomainTag::KLoop);
            return std::make_unique<parser::WhileStmt>(
                    false, is_until,
                    std::move(cond), std::move(body)
            );
        }

        auto body = Statements();
        Expect(DomainTag::KLoop);

        if (sym->GetTag() == DomainTag::KWhile || sym->GetTag() == DomainTag::KUntil) {
            bool is_until = (sym->GetTag() == DomainTag::KUntil);
            sym = scanner->NextToken();
            auto cond = Expr();
            return std::make_unique<parser::WhileStmt>(
                    true, is_until,
                    std::move(cond), std::move(body)
            );
        }

        return std::make_unique<parser::WhileStmt>(
                false, false,
                nullptr, std::move(body)
        );
    }

    // ForStmt ::= 'For' VarDef '=' Expr 'To' Expr Statements 'Next' VarDef
    std::unique_ptr<ForStmt> Parser::ForStmt() {
        Expect(DomainTag::KFor);
        auto var  = VarDef();
        Expect(DomainTag::Assign);
        auto from = Expr();
        Expect(DomainTag::KTo);
        auto to   = Expr();
        auto body = Statements();
        Expect(DomainTag::KNext);
        auto ctrl = VarDef();
        return std::make_unique<parser::ForStmt>(
                std::move(var->name), std::move(from),
                std::move(to), std::move(body)
        );
    }

    // DimStmt ::= 'Dim' Factor
    std::unique_ptr<DimStmt> Parser::DimStmt() {
        Expect(DomainTag::KDim);
        auto var = Factor();
        return std::make_unique<parser::DimStmt>(std::move(var));
    }

    // Expr ::= ArithmExpr ( RelOp ArithmExpr )?
    std::unique_ptr<Expr> Parser::Expr() {
        auto left = ArithmExpr();
        if (sym->GetTag() == DomainTag::RelOp) {
            auto op = ExpectAndCast<lexer::SpecToken>(DomainTag::RelOp);
            auto right  = ArithmExpr();
            return std::make_unique<parser::Binary>(
                    std::move(left),
                    op->GetVal(),
                    std::move(right)
            );
        }
        return left;
    }

    // ArithmExpr ::= ('+' | '-')? Term ( AddOp Term )*
    std::unique_ptr<Expr> Parser::ArithmExpr() {
        bool has_sign = (sym->GetTag() == DomainTag::Plus || sym->GetTag() == DomainTag::Minus);
        DomainTag sign;

        if (has_sign) {
            auto s = ExpectAndCast<lexer::SpecToken>(sym->GetTag());
            sign = s->GetTag();
        }

        auto expr = Term();
        if (has_sign) {
            expr = std::make_unique<Unary>(sign, std::move(expr));
        }

        while (sym->GetTag() == DomainTag::Plus || sym->GetTag() == DomainTag::Minus) {
            auto op = ExpectAndCast<lexer::SpecToken>(sym->GetTag());
            auto rhs = Term();
            expr = std::make_unique<Binary>(std::move(expr), op->GetVal(), std::move(rhs));
        }
        return expr;
    }

    // Term ::= Factor ( MulOp Factor )*
    std::unique_ptr<Expr> Parser::Term() {
        auto expr = Factor();
        while (sym->GetTag() == DomainTag::MulOp) {
            auto op = ExpectAndCast<lexer::SpecToken>(DomainTag::MulOp);
            auto rhs = Factor();
            expr = std::make_unique<Binary>(std::move(expr), op->GetVal(), std::move(rhs));
        }
        return expr;
    }

    // Factor ::= IDENT Type? ('(' Params? ')' | '[' Expr ']')?
    //          | Const
    //          | '(' Expr ')'
    std::unique_ptr<Expr> Parser::Factor()
    {
        switch (sym->GetTag()) {
            case DomainTag::Ident: {
                auto ident_tok = ExpectAndCast<lexer::IdentToken>(DomainTag::Ident);

                std::string type_mark;
                if (sym->GetTag() == DomainTag::Type) {
                    type_mark = ExpectAndCast<lexer::SpecToken>(DomainTag::Type)->GetVal();
                }

                auto node = std::make_unique<Var>(ident_tok->GetVal(), type_mark);

                if (sym->GetTag() == DomainTag::LeftParen) {
                    Expect(DomainTag::LeftParen);
                    auto params = (sym->GetTag() == DomainTag::RightParen)
                                  ? std::vector<ExprPtr>{}
                                  : Params();
                    Expect(DomainTag::RightParen);
                    return std::make_unique<parser::Call>(
                               std::move(node->name),
                         std::move(params)
                    );
                } else if (sym->GetTag() == DomainTag::LeftBracket) {
                    Expect(DomainTag::LeftBracket);
                    auto idx = Expr();
                    Expect(DomainTag::RightBracket);
                    return std::make_unique<IndexedVar>(
                            std::move(node->name),
                     std::move(idx)
                    );
                }
                return node;
            }
            case DomainTag::IntConst:
            case DomainTag::RealConst:
            case DomainTag::StringConst: {
                return Const();
            }
            case DomainTag::LeftParen: {
                sym = scanner->NextToken();
                auto e = Expr();
                Expect(DomainTag::RightParen);
                return e;
            }
            default: {
                ThrowParseError({
                    DomainTag::Ident, DomainTag::IntConst,
                    DomainTag::RealConst, DomainTag::StringConst,
                    DomainTag::LeftParen
                });
            }
        }
    }

    // Const ::= INT_CONST | REAL_CONST | STRING_CONST
    std::unique_ptr<Expr> Parser::Const() {
        switch (sym->GetTag()) {
            case DomainTag::IntConst: {
                auto tok = ExpectAndCast<lexer::IntConstToken>(DomainTag::IntConst);
                return std::make_unique<ConstInt>(tok->GetVal());
            }
            case DomainTag::RealConst: {
                auto tok = ExpectAndCast<lexer::RealConstToken>(DomainTag::RealConst);
                return std::make_unique<ConstReal>(tok->GetVal());
            }
            case DomainTag::StringConst: {
                auto tok = ExpectAndCast<lexer::StringConstToken>(DomainTag::StringConst);
                return std::make_unique<ConstString>(tok->GetVal());
            }
            default: {
                ThrowParseError({
                    DomainTag::IntConst,
                    DomainTag::RealConst,
                    DomainTag::StringConst
                });
            }
        }
    }
}
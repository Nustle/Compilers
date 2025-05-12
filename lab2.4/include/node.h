#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <boost/json.hpp>
#include "scanner.h"

namespace parser {
    class JsonNode {
    public:
        virtual ~JsonNode() = default;
        virtual boost::json::value ToJson() const = 0;
    };

    class Expr : public JsonNode { };
    using ExprPtr = std::unique_ptr<Expr>;

    class ConstInt : public Expr {
    public:
        int val;
        explicit ConstInt(int val) : val(val) {}
        boost::json::value ToJson() const override;
    };

    class ConstReal : public Expr {
    public:
        double val;
        explicit ConstReal(double val) : val(val) {}
        boost::json::value ToJson() const override;
    };

    class ConstString : public Expr {
    public:
        std::string val;
        explicit ConstString(std::string val) : val(std::move(val)) {}
        boost::json::value ToJson() const override;
    };

    class Var : public Expr {
    public:
        std::string name;
        std::string type;
        Var(std::string name, std::string type)
        : name(std::move(name)), type(std::move(type)) {}
        boost::json::value ToJson() const override;
    };

    class IndexedVar : public Expr {
    public:
        std::string name;
        ExprPtr index;
        IndexedVar(std::string name, ExprPtr index)
        : name(std::move(name)), index(std::move(index)) {}
        boost::json::value ToJson() const override;
    };

    class Call : public Expr {
    public:
        std::string name;
        std::vector<ExprPtr> args;
        Call(std::string name, std::vector<ExprPtr>&& args)
        : name(std::move(name)), args(std::move(args)) {}
        boost::json::value ToJson() const override;
    };

    class Unary : public Expr {
    public:
        lexer::DomainTag op;
        ExprPtr operand;
        Unary(lexer::DomainTag op, ExprPtr expr) : op(op), operand(std::move(expr)) {}
        boost::json::value ToJson() const override;
    };

    class Binary : public Expr {
    public:
        std::string op;
        ExprPtr lhs, rhs;
        Binary(ExprPtr left, std::string op, ExprPtr right)
        : op(op), lhs(std::move(left)), rhs(std::move(right)){}
        boost::json::value ToJson() const override;
    };

    class Stmt : public JsonNode { };
    using StmtPtr = std::unique_ptr<Stmt>;

    class AssignStmt : public Stmt {
    public:
        ExprPtr lhs, rhs;
        AssignStmt(ExprPtr left, ExprPtr right)
        : lhs(std::move(left)), rhs(std::move(right)) {}
        boost::json::value ToJson() const override;
    };

    class IfStmt : public Stmt {
    public:
        ExprPtr cond;
        std::vector<StmtPtr> then_stmt, else_stmt;
        IfStmt(ExprPtr cond, std::vector<StmtPtr>&& then_stmt, std::vector<StmtPtr>&& else_stmt)
        : cond(std::move(cond)), then_stmt(std::move(then_stmt)), else_stmt(std::move(else_stmt)) {}
        boost::json::value ToJson() const override;
    };

    class WhileStmt : public Stmt {
    public:
        bool pre_cond;
        bool until;
        ExprPtr cond;
        std::vector<StmtPtr> body;
        WhileStmt(bool pre_cond, bool until, ExprPtr cond, std::vector<StmtPtr>&& body)
        : pre_cond(pre_cond), until(until), cond(std::move(cond)), body(std::move(body)){}
        boost::json::value ToJson() const override;
    };

    class DimStmt : public Stmt {
    public:
        ExprPtr var;
        explicit DimStmt(ExprPtr var) : var(std::move(var)) {}
        boost::json::value ToJson() const override;
    };

    class ForStmt : public Stmt {
    public:
        std::string var;
        ExprPtr from, to;
        std::vector<StmtPtr> body;
        ForStmt(std::string var, ExprPtr from, ExprPtr to, std::vector<StmtPtr>&& body)
        : var(std::move(var)), from(std::move(from)), to(std::move(to)), body(std::move(body)) {}
        boost::json::value ToJson() const override;
    };

    struct Param {
        ExprPtr expr;
    };

    class Function : public JsonNode {
    public:
        bool is_sub;
        std::string name;
        std::string return_type_mark;
        std::vector<ExprPtr> params;
        std::vector<StmtPtr> body;
        Function(bool is_sub, std::string name, std::string type, std::vector<ExprPtr>&& params, std::vector<StmtPtr>&& body)
        : is_sub(is_sub), name(std::move(name)), return_type_mark(std::move(type)), params(std::move(params)), body(std::move(body)) {}
        boost::json::value ToJson() const override;
    };

    class Program : public JsonNode {
    public:
        std::vector<std::unique_ptr<Function> > funcs;
        std::vector<StmtPtr> main_body;
        Program(std::vector<std::unique_ptr<Function>>&& funcs, std::vector<StmtPtr>&& body)
        : funcs(std::move(funcs)), main_body(std::move(body)) {}
        boost::json::value ToJson() const override;
    };
}

#endif
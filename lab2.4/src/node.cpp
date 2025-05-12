#include "include/node.h"
#include <queue>

using boost::json::value;
using boost::json::object;
using boost::json::array;

namespace parser {
    value ConstInt::ToJson() const {
        return {
            {"kind", "int"},
            {"value", value(val)}
        };
    }

    value ConstReal::ToJson() const {
        return {
                {"kind", "real"},
                {"value", value(val)}
        };
    }

    value ConstString::ToJson() const {
        return {
                {"kind", "string"},
                {"value", value(val)}
        };
    }

    value Var::ToJson() const {
        return {
                {"kind", "var"},
                {"name", value(name), {"type", value(type)}}
        };
    }

    value IndexedVar::ToJson() const {
        return {
            {"kind", "index"},
            {"name", name}, {"index", index->ToJson()}
        };
    }

    value Call::ToJson() const {
        array args_arr;
        for (auto& arg : args) {
            args_arr.emplace_back(arg->ToJson());
        }

        return {
                {"kind", "call"},
                {"name", name},
                {"args", std::move(args_arr)}
        };
    }

    value Unary::ToJson() const {
        return {
            {"kind", "unary"},
            {"op", lexer::ToString(op)}, {"arg", operand->ToJson()}
        };
    }

    value Binary::ToJson() const {
        return {
            {"kind", "binary"}, {"op", op},
            {"lhs", lhs->ToJson()}, {"rhs", rhs->ToJson()}
        };
    }

    value AssignStmt::ToJson() const {
        return {
            {"kind", "assign"}, {"lhs", lhs->ToJson()}, {"rhs", rhs->ToJson()}
        };
    }

    value IfStmt::ToJson() const {
        array then_args;
        for (auto& st: then_stmt) {
            then_args.emplace_back(st->ToJson());
        }
        array else_args;
        for (auto& st: else_stmt) {
            else_args.emplace_back(st->ToJson());
        }
        return {
            {"kind", "if"}, {"cond", cond->ToJson()},
            {"then", std::move(then_args)}, {"else", std::move(else_args)}
        };
    }

    value WhileStmt::ToJson() const {
        array body_args;
        for (auto& st: body) {
            body_args.emplace_back(st->ToJson());
        }

        return {
            {"kind", "while"}, {"pre_cond", pre_cond},
            {"until", until}, {"cond", cond->ToJson()},
            {"body", std::move(body_args)}
        };
    }

    value DimStmt::ToJson() const {
        return {
            {"kind", "dim"}, {"var", var->ToJson()}
        };
    }

    value ForStmt::ToJson() const {
        array body_args;
        for (auto& st: body) {
            body_args.emplace_back(st->ToJson());
        }
        return {
            {"kind", "for"}, {"var", var}, {"from", from->ToJson()},
            {"to", to->ToJson()}, {"body", std::move(body_args)}
        };
    }

    value Function::ToJson() const {
        array params_args;
        for (auto& p: params) {
            params_args.emplace_back(p->ToJson());
        }
        array body_args;
        for (auto& s: body) {
            body_args.emplace_back(s->ToJson());
        }
        return {
            {"kind", is_sub ? "sub" : "function"}, {"name", name},
            {"ret_mark", return_type_mark}, {"params", std::move(params_args)},
            {"body", std::move(body_args)}
        };
    }

    value Program::ToJson() const {
        array funcs_args;
        for (auto& f: funcs) {
            funcs_args.emplace_back(f->ToJson());
        }
        array body_args;
        for (auto& st: main_body) {
            body_args.emplace_back(st->ToJson());
        }
        return {
            {"functions", std::move(funcs_args)}, {"statements", std::move(body_args)}
        };
    }
}
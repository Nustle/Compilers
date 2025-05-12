#ifndef TOKEN_H
#define TOKEN_H

#include "fragment.h"

namespace lexer {

    enum class DomainTag {
        LeftParen,
        RightParen,
        LeftBracket,
        RightBracket,
        Comma,
        Assign,
        Type,
        Plus,
        Minus,
        MulOp,
        RelOp,
        Ident,
        KFunction,
        KEnd,
        KSub,
        KIf,
        KThen,
        KElse,
        KDo,
        KWhile,
        KLoop,
        KUntil,
        KFor,
        KDim,
        KTo,
        KNext,
        IntConst,
        RealConst,
        StringConst,
        EndOfProgram,
    };

    inline std::string ToString(const DomainTag tag) {
        switch (tag) {
            case DomainTag::LeftParen: return "LEFT_PAREN";
            case DomainTag::RightParen: return "RIGHT_PAREN";
            case DomainTag::LeftBracket: return "LEFT_BRACKET";
            case DomainTag::RightBracket: return "RIGHT_BRACKET";
            case DomainTag::Comma: return "COMMA";
            case DomainTag::Assign: return "ASSIGN";
            case DomainTag::Type: return "TYPE";
            case DomainTag::Plus: return "PLUS";
            case DomainTag::Minus: return "MINUS";
            case DomainTag::MulOp: return "MUL_OP";
            case DomainTag::RelOp: return "REL_OP";
            case DomainTag::Ident: return "IDENT";
            case DomainTag::KFunction: return "FUNCTION";
            case DomainTag::KEnd: return "END";
            case DomainTag::KSub: return "SUB";
            case DomainTag::KIf: return "IF";
            case DomainTag::KThen: return "THEN";
            case DomainTag::KElse: return "ELSE";
            case DomainTag::KDo: return "DO";
            case DomainTag::KWhile: return "WHILE";
            case DomainTag::KLoop: return "LOOP";
            case DomainTag::KUntil: return "UNTIL";
            case DomainTag::KFor: return "FOR";
            case DomainTag::KDim: return "DIM";
            case DomainTag::KTo: return "TO";
            case DomainTag::KNext: return "NEXT";
            case DomainTag::IntConst: return "INT_CONST";
            case DomainTag::RealConst: return "REAL_CONST";
            case DomainTag::StringConst: return "STRING_CONST";
            case DomainTag::EndOfProgram: return "EOF";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, const DomainTag &tag) {
        return os << lexer::ToString(tag);
    }

    class Token {
    public:
        virtual ~Token() = default;
        DomainTag GetTag() const {
            return Tag;
        }
        Fragment GetCoords() const {
            return Coords;
        }

    protected:
        const DomainTag Tag;
        Fragment Coords;
        Token(DomainTag tag, const Position &starting, const Position &following)
        : Tag(tag), Coords(starting, following) {}
    };

    class IdentToken : public Token {
    public:
        IdentToken(const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::Ident, starting, following) {}

        const std::string &GetVal() {
            return val;
        }
    private:
        std::string val;
    };

    class KeywordToken : public Token {
    public:
        KeywordToken(DomainTag tag, const Position &starting, const Position &following)
        : Token(tag, starting, following) {}
    };

    class IntConstToken : public Token {
    public:
        IntConstToken(int val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::IntConst, starting, following) {}

        int GetVal() const {
            return val;
        }
    private:
        int val;
    };

    class RealConstToken : public Token {
    public:
        RealConstToken(double val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::RealConst, starting, following) {}

        double GetVal() const {
            return val;
        }
    private:
        double val;
    };

    class StringConstToken : public Token {
    public:
        StringConstToken(const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::StringConst, starting, following) {}

        const std::string &GetVal() {
            return val;
        }
    private:
        std::string val;
    };

    class SpecToken : public Token {
    public:
        SpecToken(DomainTag tag, const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(tag, starting, following) {}

        const std::string &GetVal() {
            return val;
        }
    private:
        std::string val;
    };

    class EOFToken : public Token {
    public:
        EOFToken(const Position &starting, const Position &following)
        : Token(DomainTag::EndOfProgram, starting, following) {}
    };
}

#endif

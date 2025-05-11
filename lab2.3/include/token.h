#ifndef TOKEN_H
#define TOKEN_H

#include "fragment.h"

namespace lexer {

    enum class DomainTag {
        NonTerminal,
        Terminal,
        Colon,
        Eps,
        LeftAngle,
        RightAngle,
        LeftBrace,
        RightBrace,
        Comma,
        EndOfProgram,
    };

    inline std::ostream& operator<<(std::ostream& os, const DomainTag &tag) {
        switch (tag) {
            case DomainTag::NonTerminal: return os << "NON_TERMINAL";
            case DomainTag::Terminal: return os << "TERMINAL";
            case DomainTag::Colon: return os << "COLON";
            case DomainTag::Eps: return os << "EPS";
            case DomainTag::LeftAngle: return os << "LEFT_ANGLE";
            case DomainTag::RightAngle: return os << "RIGHT_ANGLE";
            case DomainTag::LeftBrace: return os << "LEFT_BRACE";
            case DomainTag::RightBrace: return os << "RIGHT_BRACE";
            case DomainTag::Comma: return os << "COMMA";
            case DomainTag::EndOfProgram: return os << "EOF";
        }
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

    class TerminalToken : public Token {
    public:
        TerminalToken(const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::Terminal, starting, following) {}

        const std::string &GetVal() {
            return val;
        }
    private:
        std::string val;
    };


    class NonTerminalToken : public Token {
    public:
        NonTerminalToken(const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::NonTerminal, starting, following) {}

        const std::string &GetVal() {
            return val;
        }
    private:
        std::string val;
    };

    class SpecToken : public Token {
    public:
        SpecToken(DomainTag tag, const Position &starting, const Position &following)
        : Token(tag, starting, following) {}
    };

    class EOFToken : public Token {
    public:
        EOFToken(const Position &starting, const Position &following)
        : Token(DomainTag::EndOfProgram, starting, following) {}
    };

    inline std::ostream& operator<<(std::ostream& os, const Token &token) {
        switch (token.GetTag()) {
            case DomainTag::Terminal: {
                TerminalToken terminal = dynamic_cast<const TerminalToken&>(token);
                return os << terminal.GetTag() << " " << terminal.GetCoords() << ": " << terminal.GetVal();
            }
            case DomainTag::NonTerminal: {
                NonTerminalToken non_terminal = dynamic_cast<const NonTerminalToken&>(token);
                return os << non_terminal.GetTag() << " " << non_terminal.GetCoords() << ": "  << non_terminal.GetVal();
            }
            case DomainTag::EndOfProgram: {
                return os << token.GetTag() << " " << token.GetCoords();
            }
            default: {
                os << token.GetTag() << " " << token.GetCoords() << ": ";
                switch (token.GetTag()) {
                    case DomainTag::Colon: return os << ":";
                    case DomainTag::Eps: return os << "@";
                    case DomainTag::LeftAngle: return os << "<";
                    case DomainTag::RightAngle: return os << ">";
                    case DomainTag::LeftBrace: return os << "{";
                    case DomainTag::RightBrace: return os << "}";
                    case DomainTag::Comma: return os << ",";
                    default: return os << token.GetTag() << " " << token.GetCoords();
                }
            }
        }
    }
}

#endif

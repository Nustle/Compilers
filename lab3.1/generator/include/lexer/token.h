#ifndef TOKEN_H
#define TOKEN_H

#include "include/lexer/fragment.h"

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

    inline std::string ToStringTag(const DomainTag tag) {
        switch (tag) {
            case DomainTag::NonTerminal: return "NonTerminal";
            case DomainTag::Terminal: return "Terminal";
            case DomainTag::Colon: return "Colon";
            case DomainTag::Eps: return "Eps";
            case DomainTag::LeftAngle: return "LeftAngle";
            case DomainTag::RightAngle: return "RightAngle";
            case DomainTag::LeftBrace: return "LeftBrace";
            case DomainTag::RightBrace: return "RightBrace";
            case DomainTag::Comma: return "Comma";
            case DomainTag::EndOfProgram: return "EOF";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, const DomainTag &tag) {
        return os << lexer::ToStringTag(tag);
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

        const std::string &GetVal() const {
            return val;
        }
    private:
        std::string val;
    };


    class NonTerminalToken : public Token {
    public:
        NonTerminalToken(const std::string &val, const Position &starting, const Position &following)
        : val(val), Token(DomainTag::NonTerminal, starting, following) {}

        const std::string &GetVal() const {
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

    inline std::string ToStringToken(const Token &token) {
        switch (token.GetTag()) {
            case DomainTag::EndOfProgram: return "EOF";
            case DomainTag::Terminal: {
                TerminalToken terminal = dynamic_cast<const TerminalToken &>(token);
                if (terminal.GetVal() == ",") {
                    return "Comma";
                } else if (terminal.GetVal() == ":") {
                    return "Colon";
                } else if (terminal.GetVal() == "<") {
                    return "LeftAngle";
                } else if (terminal.GetVal() == ">") {
                    return "RightAngle";
                } else if (terminal.GetVal() == "@") {
                    return "Eps";
                } else if (terminal.GetVal() == "{") {
                    return "LeftBrace";
                } else if (terminal.GetVal() == "}") {
                    return "RightBrace";
                }
                return terminal.GetVal();
            }
            case DomainTag::NonTerminal: {
                NonTerminalToken non_terminal = dynamic_cast<const NonTerminalToken &>(token);
                return non_terminal.GetVal();
            }
            default: {
                return ToStringTag(token.GetTag());
            }
        }
    }

    inline std::ostream& operator<<(std::ostream& os, const Token &token) {
        os << token.GetTag() << " " << token.GetCoords();
        os << ": " << ToStringToken(token);
        return os;
    }
}

#endif

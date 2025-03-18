#ifndef TOKEN_H
#define TOKEN_H

#include "fragment.h"

enum class DomainTag {
    Ident = 0,
    StringConst = 1,
    END_OF_FILE = 2
};

class Token {
public:
    virtual ~Token() = 0;
    DomainTag Tag;
    Fragment Coords;
protected:
    Token(DomainTag tag, Position starting, Position following) : Coords(starting, following) {
        Tag = tag;
    }
};

Token::~Token() = default;

class IdentToken : public Token {
public:
    int code;
    IdentToken(int in_code, DomainTag tag, Position starting, Position following) : Token(tag, starting, following) {
        code = in_code;
    }
};

class StringToken : public Token {
public:
    string val;
    StringToken(const string& in_val, DomainTag tag, Position starting, Position following) : Token(tag, starting, following) {
        val = in_val;
    }
};

class EOFToken : public Token {
public:
    EOFToken(DomainTag tag, Position starting, Position following) : Token(tag, starting, following) {}
};

#endif

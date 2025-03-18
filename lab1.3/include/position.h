#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <ostream>

using namespace std;

class Position {
public:
    int textSize;
    Position(const string& text);
    int Line() const;
    int Pos() const;
    int Index() const;
    char Pp();
    char Cp();
    char Np();
    bool IsWhiteSpace();
    bool IsNewLine();
    bool IsLetterOrDigit();
    bool IsDigit();
    bool operator<(const Position& other) const {
        return index < other.index;
    }
    bool operator==(const Position& other) const {
        return index == other.index;
    }
    Position& operator++(int) {
        if (index < textSize) {
            if (IsNewLine()) {
                if (text[index] == '\r') {
                    index++;
                }
                line++;
                pos = 1;
            } else {
                pos++;
            }
            index++;
        }
        return *this;
    }
    friend ostream& operator<<(ostream& os, const Position& p) {
        os << "(" << p.line << ", " << p.pos << ")";
        return os;
    };

private:
    string text;
    int line;
    int pos;
    int index;
};

#endif
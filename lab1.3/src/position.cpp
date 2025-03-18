#include "include/position.h"
#include <cctype>

Position::Position(const string &text) {
    this->text = text;
    line = pos = 1;
    index = 0;
    textSize = (int)text.size();
}

int Position::Line() const {
    return line;
}

int Position::Pos() const {
    return pos;
}

int Position::Index() const {
    return index;
}

char Position::Pp() {
    return (index - 1 == textSize || index == 0) ? -1 : text[index - 1];
}

char Position::Cp() {
    return (index == textSize) ? -1 : text[index];
}

char Position::Np() {
    return (index + 1 == textSize) ? -1 : text[index + 1];
}

bool Position::IsWhiteSpace() {
    return index != textSize && this->Cp() == ' ';
}

bool Position::IsNewLine() {
    if (index == textSize) {
        return true;
    }
    if (text[index] == '\r' && index + 1 < textSize) {
        return text[index + 1] == '\n';
    }
    return text[index] == '\n';
}

bool Position::IsLetterOrDigit() {
    return index != textSize && (isalpha(this->Cp()) || this->IsDigit());
}

bool Position::IsDigit() {
    return index != textSize && isdigit(this->Cp());
}
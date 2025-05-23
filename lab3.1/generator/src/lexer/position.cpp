#include "include/lexer/position.h"

namespace lexer {
    bool Position::IsNewLine() {
        if (EndOfProgram()) {
            return true;
        }
        if ((*program)[index] == '\r' && index + 1 < program->size()) {
            return (*program)[index + 1] == '\n';
        }
        return (*program)[index] == '\n';
    }

    bool Position::EndOfProgram() {
        return index == program->size();
    }

    int Position::GetLine() const {
        return line;
    }
    int Position::GetPos() const {
        return pos;
    }
    int Position::GetIndex() const {
        return index;
    }
    void Position::AdvanceLine() {
        line++;
    }
    void Position::AdvancePos() {
        pos++;
    }
    void Position::AdvanceIndex() {
        index++;
    }
    void Position::PosDefault() {
        pos = 1;
    }
}
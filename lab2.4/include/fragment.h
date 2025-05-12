#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "position.h"

namespace lexer {

    class Fragment {
    public:
        Position Starting;
        Position Ending;

        Fragment(const Position &starting, const Position &ending)
        : Starting(starting), Ending(ending) {}

        friend std::ostream &operator<<(std::ostream &os, const Fragment &f) {
            os << f.Starting << "-" << f.Ending;
            return os;
        };
    };
}

#endif
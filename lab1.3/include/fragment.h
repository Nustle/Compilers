#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "position.h"

class Fragment {
public:
    Position Starting;
    Position Ending;
    Fragment(const Position& starting, Position ending) : Starting(starting), Ending(ending) {}
    friend ostream& operator<<(ostream& os, const Fragment& f) {
        os << f.Starting << "-" << f.Ending;
        return os;
    };
};

#endif
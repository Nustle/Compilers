#ifndef COMPILER_H
#define COMPILER_H

#include <map>

class Compiler {
public:
    Compiler();
    int AddName(const string& name);
    string GetName(int code);
    void AddMessage(const Position& c, const string& text);
    void OutputMessages();
private:
    map<Position, string> messages;
    map<string, int> nameCodes;
    vector<string> names;
};

#endif
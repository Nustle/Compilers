#include "include/compiler.h"

#include <iostream>

using namespace std;

Compiler::Compiler() {
    messages = map<Position, string>();
    nameCodes = map<string, int>();
    names = vector<string>();
}

int Compiler::AddName(const string& name) {
    auto nameCode = nameCodes.find(name);
    if (nameCode != nameCodes.end()) {
        return nameCode->second;
    } else {
        int code = (int)names.size();
        names.push_back(name);
        nameCodes[name] = code;
        return code;
    }
}

string Compiler::GetName(int code) {
    return names[code];
}

void Compiler::AddMessage(const Position& c, const string& text) {
    messages[c] = text;
}

void Compiler::OutputMessages() {
    for (const auto& message : messages) {
        cout << message.first << ": " << message.second << endl;
    }
}

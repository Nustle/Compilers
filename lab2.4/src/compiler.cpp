#include "include/compiler.h"

#include <iostream>

namespace lexer {

    int Compiler::AddName(const std::string &name) {
        auto nameCode = nameCodes.find(name);
        if (nameCode != nameCodes.end()) {
            return nameCode->second;
        } else {
            int code = (int) names.size();
            names.push_back(name);
            nameCodes[name] = code;
            return code;
        }
    }

    const std::string &Compiler::GetName(int code) {
        return names[code];
    }

    void Compiler::AddMessage(Position pos, MessageType type, const std::string &text) {
        messages[pos] = Message(type, text);
    }

    void Compiler::OutputMessages() {
        for (const auto &message: messages) {
            std::cout << message.first << ": " << message.second << std::endl;
        }
    }

}
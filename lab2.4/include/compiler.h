#ifndef COMPILER_H
#define COMPILER_H

#include <map>
#include <vector>
#include "message.h"
#include "position.h"

namespace lexer {

    class Compiler {
    public:
        Compiler() {
            messages = std::map<Position, Message>();
            nameCodes = std::map<std::string, int>();
            names = std::vector<std::string>();
        }

        int AddName(const std::string &name);

        const std::string &GetName(int code);

        void AddMessage(Position pos, MessageType type, const std::string &text);

        void OutputMessages();

    private:
        std::map <Position, Message> messages;
        std::map<std::string, int> nameCodes;
        std::vector <std::string> names;
    };
}

#endif
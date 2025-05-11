#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

namespace lexer {

    enum class MessageType {
        Error,
    };

    class Message {
    public:
        MessageType type;
        std::string text;

        Message() = default;

        Message(MessageType type, const std::string &text)
        : type(type), text(text) {}

        friend std::ostream& operator<<(std::ostream& os, const Message &message) {
            std::string msg_type;
            switch (message.type) {
                case MessageType::Error: {
                    msg_type = "Error";
                }
            }
            std::string sep = " ";
            os << msg_type << sep << message.text;
            return os;
        }

    };

}

#endif

#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

using namespace std;

class Message {
public:
    bool isError;
    string text;
    Message(bool in_isError, const string& in_text) {
        isError = in_isError;
        text = in_text;
    }
};

#endif

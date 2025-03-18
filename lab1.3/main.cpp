#include <iostream>
#include <fstream>
#include "src/scanner.cpp"
#include "src/position.cpp"
#include "src/compiler.cpp"

using namespace std;

int main() {
    ifstream file("/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab1.3/program.txt");
    string programText((istreambuf_iterator<char>(file)),
                      (istreambuf_iterator<char>()));
    file.close();

    Compiler compiler;
    Scanner scanner(programText, &compiler);
    unique_ptr<Token> tokenPtr = scanner.NextToken();
    while (tokenPtr->Tag != DomainTag::END_OF_FILE) {
        if (tokenPtr->Tag == DomainTag::Ident) {
            auto* identPtr = dynamic_cast<IdentToken*>(tokenPtr.get());
            cout << "IDENT " << identPtr->Coords << ": " << identPtr->code << endl;
        } else if (tokenPtr->Tag == DomainTag::StringConst) {
            auto* stringConstPtr = dynamic_cast<StringToken*>(tokenPtr.get());
            cout << "STRING " << stringConstPtr->Coords << ": " << stringConstPtr->val << endl;
        }
        tokenPtr = scanner.NextToken();
    }
    auto* EOFPtr = dynamic_cast<EOFToken*>(tokenPtr.get());
    cout << "EOF " << EOFPtr->Coords << endl;
    cout << "Messages" << endl;
    compiler.OutputMessages();
    return 0;
}
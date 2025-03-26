#include "include/scanner.h"
#include <string>

using namespace std;

Scanner::Scanner(const string &program, Compiler *in_compiler) : curr(program), compiler(in_compiler) {
    Program = program;
}

unique_ptr<Token> Scanner::NextToken() {
    bool isStringConst = false;
    Position startStringConst = curr;
    Position followingStringConst = curr;
    string valStringConst = "";
    string asciiValChar = "";

    while (curr.Cp() != -1) {
        while (curr.IsWhiteSpace() && !isStringConst) {
            curr++;
        }

        Position start = curr;

        if (!isStringConst) {
            startStringConst = curr;
        }

        switch (start.Cp()) {
            case '\'':
                curr++;
                isStringConst = true;

                while (curr.Cp() != -1 && !(curr.Cp() == '\'' && curr.Np() != '\'' && curr.Pp() != '\'')) {
                    if (!(curr.Cp() == '\'' && curr.Pp() == '\'')) {
                        valStringConst += Program[curr.Index()];
                    }
                    if (curr.IsNewLine()) {
                        compiler->AddMessage(curr, "end of line found, expected: '");
                        curr = start;
                        isStringConst = false;
                        // ~
                        valStringConst = "";
                        // ~
                        break;
                    }
                    if (curr.Cp() == '\'' && curr.Np() != '\'' && curr.Pp() == '\'') {
                        if (startStringConst.Index() + 1 == curr.Index()) {
                            compiler->AddMessage(curr, "forbidden to use ' into stringConst");
                            curr = start;
                            isStringConst = false;
                            valStringConst = "";
                            break;
                        }
                    }
                    curr++;
                }

                followingStringConst = curr++;
                break;
            case '#':
                curr++;
                isStringConst = true;
                if (!curr.IsDigit()) {
                    compiler->AddMessage(curr, "after # must be digit");
                    isStringConst = false;
                    valStringConst = "";
                    continue;
                }

                while (curr.IsDigit()) {
                    asciiValChar += Program[curr.Index()];
                    curr++;
                }

                followingStringConst = curr;
                valStringConst += static_cast<char>(stoi(asciiValChar));;
                asciiValChar = "";
                break;
            default:
                if (isStringConst) {
                    if (curr.IsNewLine()) {
                        curr++;
                    }
                    return make_unique<StringToken>(
                            valStringConst,
                            DomainTag::StringConst,
                            startStringConst,
                            followingStringConst
                    );
                }

                if (start.IsDigit()) {
                    compiler->AddMessage(curr, "identifier can't start with digit");
                } else if (!start.IsLetterOrDigit()) {
                    compiler->AddMessage(curr, "unexpected character");
                    curr++;
                    continue;
                }
                while (curr.IsLetterOrDigit()) {
                    curr++;
                }
                Position identCurr = curr;
                if (curr.IsNewLine()) {
                    curr++;
                }
                return make_unique<IdentToken>(
                        compiler->AddName(Program.substr(start.Index(), identCurr.Index() - start.Index())),
                        DomainTag::Ident,
                        start,
                        identCurr
                );
        }
    }
    if (isStringConst) {
        compiler->AddMessage(curr, "unexpected EOF in stringConst");
    }
    return make_unique<EOFToken>(
            DomainTag::END_OF_FILE,
            curr,
            curr
    );
}

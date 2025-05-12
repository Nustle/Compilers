#include <fstream>

#include "include/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Whoops: needed program.txt\n";
        return 1;
    }
    ifstream file(argv[1]);
    string programText((istreambuf_iterator<char>(file)),
                       (istreambuf_iterator<char>()));
    file.close();

    lexer::Compiler compiler;
    unique_ptr<lexer::Scanner> scanner = make_unique<lexer::Scanner>(programText, &compiler);
    parser::Parser parser(std::move(scanner));

    try {
        const unique_ptr<parser::Program> root = parser.RecursiveDescentParse();
        std::ofstream out(argv[2]);
        out << boost::json::serialize(root->ToJson());
        cout << "Saved AST tree..." << endl;
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
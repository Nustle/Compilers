#include <fstream>

#include "include/parser.h"

using namespace std;

int main() {
    ifstream file("/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab2.3/example/program.txt");
    string programText((istreambuf_iterator<char>(file)),
                      (istreambuf_iterator<char>()));
    file.close();

    lexer::Compiler compiler;
    lexer::Scanner scanner(programText, &compiler);
    parser::Parser parser = parser::Parser();

    std::ofstream output_file("/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab2.3/output/tree.txt");

    try {
        std::unique_ptr<parser::Node> root = parser.TopDownParse(&scanner);
        root->OutputTree(output_file );
        cout << "Tree saved" << endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
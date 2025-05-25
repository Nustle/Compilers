#include <fstream>
#include "include/parser/parser.h"
#include "include/semantics/interpret.h"

using namespace std;

int main() {
    const string& grammar_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/calculator/example/program.txt";
    const string& tree_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/calculator/output/tree.txt";

    ifstream file(grammar_path);
    string program_text((istreambuf_iterator<char>(file)),
                        (istreambuf_iterator<char>()));
    file.close();

    lexer::Compiler compiler;
    lexer::Scanner scanner(program_text, &compiler);
    parser::Parser parser = parser::Parser();

    std::ofstream output_file(tree_path);

    try {
        std::unique_ptr<parser::Node> root = parser.TopDownParse(&scanner);
        root->OutputTree(output_file);

        const auto& program_node = dynamic_cast<const parser::InnerNode&>(*root);
        semantics::Interpreter interpreter{};
        int ans = interpreter.Interpret(program_node);
        cout << ans << endl;
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
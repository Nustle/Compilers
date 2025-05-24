#include <fstream>

#include "include/parser/parser.h"
#include "include/semantics/table_generator.h"

using namespace std;

void GenerateCompiler(const string& input_grammar_path, const string& output_tree_path, const string& output_table_path) {
    ifstream file(input_grammar_path);
    string program_text((istreambuf_iterator<char>(file)),
                       (istreambuf_iterator<char>()));
    file.close();

    lexer::Compiler compiler;
    lexer::Scanner scanner(program_text, &compiler);
    parser::Parser parser = parser::Parser();

    std::ofstream output_file(output_tree_path);

    std::unique_ptr<parser::Node> root = parser.TopDownParse(&scanner);
    root->OutputTree(output_file );

    const auto& program_node = dynamic_cast<const parser::InnerNode&>(*root);
    auto converter_grammar = semantics::ConverterGrammar{};
    std::shared_ptr<semantics::Program> program = converter_grammar.ParseProgram(program_node);
    semantics::FirstFollow sets(program);
    semantics::TableGenerator generator(program, sets);
    generator.Generate(output_table_path);
}

int main() {
    try {
        const string& self_grammar_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/generator/example/self_program.txt";
        const string& self_tree_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/generator/output/self_tree.txt";
        const string& self_table_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/generator/src/parser/analyzer_table.cpp";
        GenerateCompiler(self_grammar_path, self_tree_path, self_table_path);

        const string& grammar_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/generator/example/program.txt";
        const string& tree_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/generator/output/tree.txt";
        const string& table_path = "/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab3.1/calculator/src/parser/table.cpp";
        GenerateCompiler(grammar_path, tree_path, table_path);
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
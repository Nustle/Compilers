#ifndef TABLE_GENERATOR_H
#define TABLE_GENERATOR_H

#include "ast.h"
#include <unordered_map>
#include <unordered_set>

namespace semantics {
    class FirstFollow {
    public:
        explicit FirstFollow(const std::shared_ptr<semantics::Program>& program) {
            BuildFirst(program);
            BuildFollow(program);
        }

        const std::unordered_map<parser::Symbol, std::unordered_set<parser::Symbol>>& GetFirst() const {
            return first;
        }

        const std::unordered_map<parser::Symbol, std::unordered_set<parser::Symbol>>& GetFollow() const {
            return follow;
        }

    private:
        void BuildFirst(const std::shared_ptr<Program>& program);
        void BuildFollow(const std::shared_ptr<Program>& program);
        std::unordered_map<parser::Symbol, std::unordered_set<parser::Symbol>> first;
        std::unordered_map<parser::Symbol, std::unordered_set<parser::Symbol>> follow;
    };

    class TableGenerator {
    public:
        TableGenerator(const std::shared_ptr<semantics::Program>& program, const FirstFollow& first_follow);
        void Generate(const std::string& out_dir) const;

        const parser::Symbol& GetAxiom() const {
            return program->GetAxiom();
        }

        const std::unordered_map<parser::Symbol, std::unordered_map<parser::Symbol, std::vector<parser::Symbol>>>& GetTable() const {
            return table;
        }
    private:
        std::unordered_map<parser::Symbol, std::unordered_map<parser::Symbol, std::vector<parser::Symbol>> > table;
        std::shared_ptr<Program> program;
    };
}

#endif

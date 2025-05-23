#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>

namespace parser {
    class Symbol {
    public:
        enum class Type {
            Terminal,
            NonTerminal,
            Spec,
        };

        Symbol(std::string name, Type type)
        : name(std::move(name)), type(type) {}

        bool operator==(const Symbol& other) const {
            return type == other.type && name == other.name;
        }

        const std::string& GetName() const {
            return name;
        }
        Type GetType() const {
            return type;
        }

    private:
        std::string name;
        Type type;
    };

    const auto Epsilon = Symbol{"ε", Symbol::Type::Spec};
    const auto EndOfProgram = Symbol{"EOF", Symbol::Type::Terminal};

    using SymbolVec    = std::vector<Symbol>;
    using SymbolVecRef = std::reference_wrapper<const SymbolVec>;
    using OptSymbolVec = std::optional<SymbolVecRef>;
}

namespace std {
    template<>
    struct hash<parser::Symbol> {
        size_t operator()(parser::Symbol const& s) const noexcept {
            auto h1 = std::hash<std::string>()(s.GetName());
            auto h2 = std::hash<int>()(static_cast<int>(s.GetType()));
            return h1 ^ (h2 << 1);
        }
    };
}

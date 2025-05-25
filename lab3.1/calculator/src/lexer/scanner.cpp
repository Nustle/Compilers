#include "include/lexer/scanner.h"
#include <string>
#include <regex>

namespace lexer {

    struct RegexDomain {
        DomainTag tag;
        std::regex pattern;

        RegexDomain(DomainTag tag, const std::regex &pattern)
                : tag(tag), pattern(pattern) {}
    };

    std::vector<RegexDomain> regexes = {
            RegexDomain(DomainTag::Number, std::regex(R"([0-9]+)")),
            RegexDomain(DomainTag::Plus, std::regex(R"(\+)")),
            RegexDomain(DomainTag::Star, std::regex(R"(\*)")),
            RegexDomain(DomainTag::LeftParen, std::regex(R"(\()")),
            RegexDomain(DomainTag::RightParen, std::regex(R"(\))")),
    };
    std::regex WhiteSpaceRegex = std::regex(R"([ \t\n\r]+)");
    std::regex CommentRegex = std::regex(R"(--[^\n]*)");


    std::unique_ptr <Token> Scanner::NextToken() {
        std::smatch m;
        auto program_rest = program.substr(cur.GetIndex());

        size_t lex_len = 0;
        DomainTag lex_tag;

        while (lex_len == 0) {
            if (cur.EndOfProgram()) {
                return std::make_unique<EOFToken>(cur, cur);
            }
            if (std::regex_search(program_rest, m, WhiteSpaceRegex, std::regex_constants::match_continuous)) {
                size_t len = m.str(0).size();
                cur += len;
            } else if (std::regex_search(program_rest, m, CommentRegex, std::regex_constants::match_continuous)) {
                size_t len = m.str(0).size();
                Position start = cur;
                start += 2;
                cur += len;
                comments.emplace_back(start, cur);
            } else {
                for (auto &rd: regexes) {
                    if (std::regex_search(program_rest, m, rd.pattern, std::regex_constants::match_continuous)) {
                        size_t len = m.str(0).size();
                        if (len > lex_len) {
                            lex_len = len;
                            lex_tag = rd.tag;
                        }
                    }
                }
                if (lex_len == 0) {
                    compiler->AddMessage(cur, MessageType::Error, "unexpected char");
                    cur++;
                }
            }

            program_rest = program.substr(cur.GetIndex());
        }

        Position start(&program), end(&program);

        start = cur;
        cur += lex_len;
        end = cur;

        std::string lex = program.substr(start.GetIndex(), end.GetIndex() - start.GetIndex());

        switch (lex_tag) {
            case DomainTag::Number: {
                return std::make_unique<NumberToken>(
                        std::stoi(lex),
                        start,
                        end
                );
            }
            default: {
                return std::make_unique<SpecToken>(
                        lex_tag,
                        start,
                        end
                );
            }
        }
    }

}
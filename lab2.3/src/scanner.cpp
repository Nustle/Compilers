#include "include/scanner.h"
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
            RegexDomain(DomainTag::NonTerminal, std::regex(R"([A-Za-z][A-Za-z0-9]*'*)")),
            RegexDomain(DomainTag::Terminal, std::regex(R"("[^\n"']+")")),
            RegexDomain(DomainTag::Colon, std::regex(R"(:)")),
            RegexDomain(DomainTag::Eps, std::regex(R"(@)")),
            RegexDomain(DomainTag::LeftAngle, std::regex(R"(<)")),
            RegexDomain(DomainTag::RightAngle, std::regex(R"(>)")),
            RegexDomain(DomainTag::LeftBrace, std::regex(R"(\{)")),
            RegexDomain(DomainTag::RightBrace, std::regex(R"(\})")),
            RegexDomain(DomainTag::Comma, std::regex(R"(,)")),
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
                            //lex = m.str(0);
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
        if (lex_tag == DomainTag::Terminal) {
            cur++;
            start = cur;
            cur += lex_len - 2;
            end = cur;
            cur++;
        } else {
            start = cur;
            cur += lex_len;
            end = cur;
        }
        std::string lex = program.substr(start.GetIndex(), end.GetIndex() - start.GetIndex());

        switch (lex_tag) {
            case DomainTag::Terminal: {
                return std::make_unique<TerminalToken>(
                        lex,
                        start,
                        end
                );
            }
            case DomainTag::NonTerminal: {
                return std::make_unique<NonTerminalToken>(
                        lex,
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
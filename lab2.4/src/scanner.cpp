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
            RegexDomain(DomainTag::Ident, std::regex(R"([A-Za-z][A-Za-z0-9]*)")),
            RegexDomain(DomainTag::LeftParen, std::regex(R"(\()")),
            RegexDomain(DomainTag::RightParen, std::regex(R"(\))")),
            RegexDomain(DomainTag::LeftBracket, std::regex(R"(\[)")),
            RegexDomain(DomainTag::RightBracket, std::regex(R"(\])")),
            RegexDomain(DomainTag::Comma, std::regex(R"(,)")),
            RegexDomain(DomainTag::Assign, std::regex(R"(=)")),
            RegexDomain(DomainTag::Type, std::regex(R"([%&!#$])")),
            RegexDomain(DomainTag::Plus, std::regex(R"(\+)")),
            RegexDomain(DomainTag::Minus, std::regex(R"(-)")),
            RegexDomain(DomainTag::MulOp, std::regex(R"([\*/])")),
            RegexDomain(DomainTag::RelOp, std::regex(R"(>=|<=|==|<>|>|<)")),
            RegexDomain(DomainTag::KFunction, std::regex(R"(function)", std::regex::icase)),
            RegexDomain(DomainTag::KEnd, std::regex(R"(end)", std::regex::icase)),
            RegexDomain(DomainTag::KSub, std::regex(R"(sub)", std::regex::icase)),
            RegexDomain(DomainTag::KIf, std::regex(R"(if)", std::regex::icase)),
            RegexDomain(DomainTag::KThen, std::regex(R"(then)", std::regex::icase)),
            RegexDomain(DomainTag::KElse, std::regex(R"(else)", std::regex::icase)),
            RegexDomain(DomainTag::KDo, std::regex(R"(do)", std::regex::icase)),
            RegexDomain(DomainTag::KWhile, std::regex(R"(while)", std::regex::icase)),
            RegexDomain(DomainTag::KLoop, std::regex(R"(loop)", std::regex::icase)),
            RegexDomain(DomainTag::KUntil, std::regex(R"(until)", std::regex::icase)),
            RegexDomain(DomainTag::KFor, std::regex(R"(for)", std::regex::icase)),
            RegexDomain(DomainTag::KDim, std::regex(R"(dim)", std::regex::icase)),
            RegexDomain(DomainTag::KTo, std::regex(R"(to)", std::regex::icase)),
            RegexDomain(DomainTag::KNext, std::regex(R"(next)", std::regex::icase)),
            RegexDomain(DomainTag::IntConst, std::regex(R"([0-9]+)")),
            RegexDomain(DomainTag::RealConst, std::regex(R"([0-9]+\.[0-9]+)")),
            RegexDomain(DomainTag::StringConst, std::regex("\"([^\"\\n]*)\"")),
    };

    std::regex WhiteSpaceRegex = std::regex(R"([ \t\n\r]+)");
    std::regex CommentRegex = std::regex(R"('[^\n]*)");


    std::unique_ptr <Token> Scanner::NextToken() {
        DomainTag keywords[] = {
                DomainTag::KFunction, DomainTag::KEnd, DomainTag::KSub,
                DomainTag::KIf, DomainTag::KThen, DomainTag::KElse,
                DomainTag::KDo, DomainTag::KWhile, DomainTag::KLoop,
                DomainTag::KUntil, DomainTag::KFor, DomainTag::KDim,
               DomainTag::KTo, DomainTag::KNext
        };

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
                start += 1;
                cur += len;
                comments.emplace_back(start, cur);
            } else {
                for (auto &rd: regexes) {
                    if (std::regex_search(program_rest, m, rd.pattern, std::regex_constants::match_continuous)) {
                        size_t len = m.str(0).size();
                        if (len > lex_len) {
                            lex_len = len;
                            lex_tag = rd.tag;
                        } else if (len == lex_len && lex_tag == DomainTag::Ident) {
                            for (auto keyword : keywords) {
                                if (rd.tag == keyword) {
                                    lex_tag = rd.tag;
                                }
                            }
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
            case DomainTag::Ident: {
                return std::make_unique<IdentToken>(
                        lex,
                        start,
                        end
                );
            }
            case DomainTag::KFunction:
            case DomainTag::KEnd:
            case DomainTag::KSub:
            case DomainTag::KIf:
            case DomainTag::KThen:
            case DomainTag::KElse:
            case DomainTag::KDo:
            case DomainTag::KWhile:
            case DomainTag::KLoop:
            case DomainTag::KUntil:
            case DomainTag::KFor:
            case DomainTag::KDim:
            case DomainTag::KTo:
            case DomainTag::KNext: {
                return std::make_unique<KeywordToken>(
                        lex_tag,
                        start,
                        end
                );
            }
            case DomainTag::IntConst: {
                return std::make_unique<IntConstToken>(
                        std::stoi(lex),
                        start,
                        end
                );
            }
            case DomainTag::RealConst: {
                return std::make_unique<RealConstToken>(
                        std::stoll(lex),
                        start,
                        end
                );
            }
            case DomainTag::StringConst: {
                return std::make_unique<StringConstToken>(
                        lex,
                        start,
                        end
                );
            }
            default: {
                return std::make_unique<SpecToken>(
                        lex_tag,
                        lex,
                        start,
                        end
                );
            }
        }
    }
}
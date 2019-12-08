/*
 * LexParser.h
 * Implemention of Lexical Parser.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef __COMPILER_LEX_PARSER_
#define __COMPILER_LEX_PARSER_

#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <utility>
#include <variant>
#include <optional>

namespace Compiler {
    // 类别表
    enum class Symbol {
        BEGIN               = 1,
        END                 = 2,
        IF                  = 3,
        THEN                = 4,
        ELSE                = 5,
        IDENTIFIER          = 20,
        INTEGER             = 21,
        PLUS                = 22,
        MINUS               = 23,
        STAR                = 24,
        DIVIDE              = 25,
        LBRACKET            = 26,
        RBRACKET            = 27,
        COMMA               = 28,
        SEMI                = 29,
        COLON               = 30,
        ASSIGN              = 31,
        EQUAL               = 32,
        COMMENT             = 33,
        INIT                = 0,
        INCOMPLETECOMMENT   = -1,
        SEOF                = -2,
        UNDEFINED           = -3,
    };

    // 重载输出运算符
    std::ostream& operator<<(std::ostream& out, Symbol symbol) {
        out << static_cast<int>(symbol);
        return out;
    }

    // 保留字表
    const auto reserved = std::unordered_map<std::string, Symbol>({
        { "BEGIN",  Symbol::BEGIN },
        { "END",    Symbol::END },
        { "IF",     Symbol::IF },
        { "THEN",   Symbol::THEN },
        { "ELSE",   Symbol::ELSE },
    });

    // 返回类型
    struct ParseResult {
        // 类别
        Symbol symbol;
        // 附加信息
        // 未定义的标识符和不完整的评论 -> string
        // 无符号整数 -> int
        std::optional<std::variant<std::string, int> > info;

        ParseResult(Symbol _symbol):
            symbol(_symbol), info(std::nullopt) { }
        ParseResult(Symbol _symbol, std::string _str):
            symbol(_symbol), info(std::make_optional(_str)) { }
        ParseResult(Symbol _symbol, int _num):
            symbol(_symbol), info(std::make_optional(_num)) { }
        ~ParseResult() { }

        auto getMsg() {
            return std::get<std::string>(info.value());
        }

        auto getNum() {
            return std::get<int>(info.value());
        }
    };

    // 词法分析器
    class LexParser {
    private:
        // 读入EOF抛出的异常
        class _EOFException { };

        std::istream &_in;  // 绑定的流 默认是stdin
        std::string _token;
        int _ch;
        Symbol _symbol;     // 存储类别 用于异常处理时的返回

        inline void init() {
            _token = "";
            _symbol = Symbol::INIT;
        }

        // 读取字符 遇到EOF抛出异常
        inline void getChar() {
            if (_ch = _in.get(); _in.eof()) {
                throw _EOFException();
            }
        }

        inline void ungetChar() {
            _in.unget();
        }

        // 判断保留字
        inline void checkReserved() {
            if (auto res = reserved.find(_token); res == reserved.end()) {
                // 标识符
                _symbol = Symbol::IDENTIFIER;
            } else {
                // 保留字
                _symbol = res->second;
            }
        }

        inline void catToken() {
            _token += _ch;
            getChar();
        }

        // 使用std::stoi判断是否溢出
        inline std::optional<int> parseInteger() {
            try {
                int num = std::stoi(_token);
                return std::make_optional<int>(num);
            } catch (std::out_of_range &e) {
                return std::nullopt;
            }
        }

        // 返回分析结果
        inline ParseResult makeResult() noexcept {
            switch (_symbol) {
                case Symbol::INIT: {
                    // 当流一开始就指向文件尾 返回EOF
                    if (_ch == -1) {
                        return { Symbol::SEOF };
                    }
                    break;
                }
                case Symbol::IDENTIFIER: {
                    // 判断是否保留字
                    if (checkReserved(); _symbol == Symbol::IDENTIFIER) {
                        return { _symbol, _token };
                    }
                    return { _symbol };
                }
                case Symbol::INTEGER: {
                    if (auto res = parseInteger(); res.has_value()) {
                        return { _symbol, res.value() };
                    }
                    return { _symbol, "OF" };
                }
                case Symbol::INCOMPLETECOMMENT:
                    return { _symbol, "incomplete comment" };
                case Symbol::UNDEFINED:
                    return { _symbol , _token };
                default:
                    return { _symbol };
            }
        }

        inline auto isSpace() { return _ch == ' '; }

        inline auto isNewLine() { return _ch == '\n' || _ch == '\r'; }

        inline auto isTab() { return _ch == '\t'; }

        inline auto isBlank() { return isSpace() || isNewLine() || isTab(); }

        inline auto isLetter() { return std::isalpha(_ch); }

        inline auto isDigit() { return std::isdigit(_ch); }

        inline auto isColon() { return _ch == ':'; }

        inline auto isComma() { return _ch == ','; }

        inline auto isSemi() { return _ch == ';'; }

        inline auto isEqu() { return _ch == '='; }

        inline auto isPlus() { return _ch == '+'; }

        inline auto isMinus() { return _ch == '-'; }

        inline auto isDivide() { return _ch == '/'; }

        inline auto isStar() { return _ch == '*'; }

        inline auto isLBracket() { return _ch == '('; }

        inline auto isRBracket() { return _ch == ')'; }

    public:
        LexParser(std::istream &in = std::cin): _in(in) { }
        ~LexParser() {}

        ParseResult parseNext() {
            init();

            try {
                do {
                    getChar();
                } while (isBlank());

                if (isLetter()) {
                    _symbol = Symbol::IDENTIFIER;
                    do {
                        catToken();
                    } while (isLetter() || isDigit());
                    ungetChar();
                }

                else if (isDigit()) {
                    _symbol = Symbol::INTEGER;
                    do { catToken(); } while (isDigit());
                    ungetChar();
                }

                else if (isEqu()) {
                    _symbol = Symbol::EQUAL;
                }

                else if (isColon()) {
                    _symbol = Symbol::COLON;

                    if (getChar(); isEqu()) {
                        _symbol = Symbol::ASSIGN;
                    } else {
                        ungetChar();
                    }
                }

                else if (isPlus()) {
                    _symbol = Symbol::PLUS;
                }

                else if (isMinus()) {
                    _symbol = Symbol::MINUS;
                }

                else if (isStar()) {
                    _symbol = Symbol::STAR;
                }

                else if (isLBracket()) {
                    _symbol = Symbol::LBRACKET;
                }

                else if (isRBracket()) {
                    _symbol = Symbol::RBRACKET;
                }

                else if (isComma()) {
                    _symbol = Symbol::COMMA;
                }

                else if (isSemi()) {
                    _symbol = Symbol::SEMI;
                }

                else if (isDivide()) {
                    _symbol = Symbol::DIVIDE;
                    getChar();

                    if (isStar()) {
                        _symbol = Symbol::INCOMPLETECOMMENT;

                        do {
                            do {
                                getChar();
                            } while (!isStar());
                            do {
                                if (getChar(); isDivide()) {
                                    return { Symbol::COMMENT };
                                }
                            } while (isStar());
                        } while (!isStar());
                    } else {
                        ungetChar();
                    }
                }

                else {
                    _symbol = Symbol::UNDEFINED;
                    _token += _ch;
                }

                return makeResult();
            } catch (_EOFException &e) {
                return makeResult();
            }
        }
    };
}

#endif
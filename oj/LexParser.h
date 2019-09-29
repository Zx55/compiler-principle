/*
 * LexParser.h
 * 词法分析器 文法见书P56
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef __COMPILER_LEX_PARSER_
#define __COMPILER_LEX_PARSER_

#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <utility>

namespace Compiler {
    // 类别表
    enum class Symbol {
        INIT,
        BEGIN,
        END,
        IF,
        THEN,
        ELSE,
        IDENTIFIER = 20,
        INTEGER,
        PLUS,
        MINUS,
        STAR,
        DIVIDE,
        LBRACKET,
        RBRACKET,
        COMMA,
        SEMI,
        COLON,
        ASSIGN,
        EQUAL,
        COMMENT,
        SEOF,
        UNDEFINED,
        INCOMPLETECOMMENT = -1,
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
        std::string info;
        // 无符号整数的数值表示
        int num;

        ParseResult(Symbol _symbol):
            symbol(_symbol), info(""), num(0) { }
        ParseResult(Symbol _symbol, std::string _info):
            symbol(_symbol), info(_info), num(0) { }
        ParseResult(Symbol _symbol, int _num):
            symbol(_symbol), info(""), num(_num) { }
        ~ParseResult() { }
    };

    // 读入EOF抛出的异常
    class EOFException { };

    // 词法分析器
    class LexParser {
    private:
        std::istream &_in;       // 绑定的流 默认是stdin
        std::string _token;
        int _ch;
        Symbol _symbol;     // 存储类别 用于异常处理时的返回

        inline void init() {
            _token = "";
            _symbol = Symbol::INIT;
        }

        // 读取字符 遇到EOF抛出异常
        inline void getChar() {
            _ch = _in.get();
            if (_in.eof()) {
                throw EOFException();
            }
        }

        inline void ungetChar() {
            _in.unget();
        }

        // 判断保留字
        inline Symbol checkReserved() {
            auto res = reserved.find(_token);

            if (res == reserved.end()) {
                // 标识符
                return Symbol::IDENTIFIER;
            } else {
                // 保留字
                return res->second;
            }
        }

        inline void catToken() {
            _token += _ch;
            getChar();
        }

        // 使用std::stoi判断是否溢出
        inline std::pair<bool, int> checkOverFlow() {
            try {
                int num = std::stoi(_token);
                return { false, num };
            } catch (std::out_of_range &e) {
                return { true, 0 };
            }
        }

        // 返回分析结果
        inline ParseResult makeResult() {
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
                    _symbol = checkReserved();
                    if (_symbol == Symbol::IDENTIFIER) {
                        return { _symbol, _token };
                    }
                    return { _symbol };
                }
                case Symbol::INTEGER: {
                    auto overFlowInfo = checkOverFlow();
                    if (overFlowInfo.first) {
                        return { _symbol, "OF" };
                    }
                    return { _symbol, overFlowInfo.second };
                }
                case Symbol::INCOMPLETECOMMENT:
                    return { _symbol, "incomplete comment" };
                case Symbol::UNDEFINED:
                    return { _symbol , _token };
                default:
                    return { _symbol };
            }
        }

        inline auto isSpace() {
            return _ch == ' ';
        }

        inline auto isNewLine() {
            return _ch == '\n' || _ch == '\r';
        }

        inline auto isTab() {
            return _ch == '\t';
        }

        inline auto isBlank() {
            return isSpace() || isNewLine() || isTab();
        }

        inline auto isLetter() {
            return isalpha(_ch);
        }

        inline auto isDigit() {
            return isdigit(_ch);
        }

        inline auto isColon() {
            return _ch == ':';
        }

        inline auto isComma() {
            return _ch == ',';
        }

        inline auto isSemi() {
            return _ch == ';';
        }

        inline auto isEqu() {
            return _ch == '=';
        }

        inline auto isPlus() {
            return _ch == '+';
        }

        inline auto isMinus() {
            return _ch == '-';
        }

        inline auto isDivide() {
            return _ch == '/';
        }

        inline auto isStar() {
            return _ch == '*';
        }

        inline auto isLBracket() {
            return _ch == '(';
        }

        inline auto isRBracket() {
            return _ch == ')';
        }

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
                    getChar();

                    if (isEqu()) {
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
                                getChar();
                                if (isDivide()) {
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
                    catToken();
                }

                return makeResult();
            } catch (EOFException &e) {
                return makeResult();
            }
        }
    };
}

#endif
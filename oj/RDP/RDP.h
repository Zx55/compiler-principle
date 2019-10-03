/*
 * RDP.h
 * Implement of Recursive Descent Parser
 * Copyright (c) zx5. All rights reserved.
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <cctype>

namespace Compiler {
    // 类别表
    enum class Symbol {
        IDENTIFIER,
        IF,
        THEN,
        ELSE,
        LPARENT,  // (
        RPARENT,  // )
        LBRACKET, // [
        RBRACKET, // ]
        PLUS,
        MULTI,
        ASSIGN,
        INIT,
        SEOF,
        UNDEFINED,
    };

    // 保留字表
    const auto reserved = std::unordered_map<std::string, Symbol>({
        { "IF",     Symbol::IF },
        { "THEN",   Symbol::THEN },
        { "ELSE",   Symbol::ELSE },
    });

    // 词法分析器
    class Lex {
    private:
        class EOFException {};

        std::istream &_in;
        std::string _token;
        int _ch;
        Symbol &_symbol;

        inline void _init() {
            _token = "";
            _symbol = Symbol::INIT;
        }

        inline void _get() {
            _ch = _in.get();
            if (_in.eof()) {
                throw EOFException();
            }
        }

        inline void _unget() { _in.unget(); }

        inline auto _isSpace() { return _ch == ' '; }

        inline auto _isNewLine() { return _ch == '\n' || _ch == '\r'; }

        inline auto _isTab() { return _ch == '\t'; }

        inline auto _isBlank() { return _isSpace() || _isNewLine() || _isTab(); }

        inline auto _isAlpha() { return std::isalpha(_ch); }

        inline auto _isLParent() { return _ch == '('; }

        inline auto _isRParent() { return _ch == ')'; }

        inline auto _isLBracket() { return _ch == '['; }

        inline auto _isRBracket() { return _ch == ']'; }

        inline auto _isPlus() { return _ch == '+'; }

        inline auto _isStar() { return _ch == '*'; }

        inline auto _isColon() { return _ch == ':'; }

        inline auto _isEqu() { return _ch == '='; }

    public:
        Lex(std::istream &in, Symbol &symbol) : _in(in), _symbol(symbol) {}
        ~Lex() = default;

        void _next() {
            _init();

            try {
                do {
                    _get();
                } while (_isBlank());

                if (_isAlpha()) {
                    _symbol = Symbol::IDENTIFIER;
                    do {
                        _token += _ch;
                        _get();
                    } while (_isAlpha());
                    _unget();

                    auto it = reserved.find(_token);
                    if (it != reserved.end()) {
                        _symbol = it->second;
                    }
                }

                else if (_isLParent()) {
                    _symbol = Symbol::LPARENT;
                }

                else if (_isRParent()) {
                    _symbol = Symbol::RPARENT;
                }

                else if (_isLBracket()) {
                    _symbol = Symbol::LBRACKET;
                }

                else if (_isRBracket()) {
                    _symbol = Symbol::RBRACKET;
                }

                else if (_isPlus()) {
                    _symbol = Symbol::PLUS;
                }

                else if (_isStar()) {
                    _symbol = Symbol::MULTI;
                }

                else if (_isColon()) {
                    _symbol = Symbol::UNDEFINED;
                    _get();
                    if (_isEqu()) {
                        _symbol = Symbol::ASSIGN;
                    } else {
                        _unget();
                    }
                }

                else {
                    _symbol = Symbol::UNDEFINED;
                }

            } catch (EOFException &e) {
                switch (_symbol) {
                    case Symbol::INIT: {
                        _symbol = Symbol::SEOF;
                        break;
                    }
                    case Symbol::IDENTIFIER: {
                        auto it = reserved.find(_token);
                        if (it != reserved.end()) {
                            _symbol = it->second;
                        }
                        break;
                    }
                }
            }
        }
    };

    class RDP {
    private:
        Lex _lex;
        Symbol _symbol;

        bool _isStatement() {
            if (_isVar()) {
                if (_symbol == Symbol::ASSIGN) {
                    _lex._next();
                    return _isExpr();
                }
                return false;
            } else if (_symbol == Symbol::IF) {
                _lex._next();
                if (_isExpr() && _symbol == Symbol::THEN) {
                    _lex._next();
                    if (_isStatement()) {
                        if (_symbol == Symbol::ELSE) {
                            _lex._next();
                            return _isStatement();
                        }
                        return true;
                    }
                    return false;
                }
                return false;
            }

            return false;
        }

        bool _isExpr() {
            if (_isTerm()) {
                while (_symbol == Symbol::PLUS) {
                    _lex._next();
                    if (!_isTerm()) {
                        return false;
                    }
                }
                return true;
            }

            return false;
        }

        bool _isTerm() {
            if (_isFactor()) {
                while (_symbol == Symbol::MULTI) {
                    _lex._next();
                    if (!_isFactor()) {
                        return false;
                    }
                }
                return true;
            }

            return false;
        }

        bool _isFactor() {
            if (_isVar()) {
                return true;
            } else if (_symbol == Symbol::LPARENT) {
                _lex._next();
                if (_isExpr() && _symbol == Symbol::RPARENT) {
                    _lex._next();
                    return true;
                }
                return false;
            }

            return false;
        }

        bool _isVar() {
            if (_symbol == Symbol::IDENTIFIER) {
                _lex._next();
                if (_symbol == Symbol::LBRACKET) {
                    _lex._next();
                    if (_isExpr() && _symbol == Symbol::RBRACKET) {
                        _lex._next();
                        return true;
                    }
                    return false;
                }
                return true;
            }

            return false;
        }

    public:
        RDP(std::istream &in = std::cin) : _lex(in, _symbol) {}
        ~RDP() = default;

        bool parse() {
            _lex._next();
            return _isStatement();
        }
    };
}

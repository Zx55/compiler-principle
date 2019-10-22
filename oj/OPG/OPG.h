/*
 * OPG.h
 * Implemention of OPG Parser.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef __COMPILER_OPG_PARSER_
#define __COMPILER_OPG_PARSER_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Compiler {
    using Symbol = char;
    // 0 => 非终结符
    const Symbol SYM_VN = 0;
    // -1 => 错误
    const Symbol SYM_ERROR = -1;
    const Symbol SYM_SEPARATOR = '#';

    enum class Precedence {
        EQUAL,
        BELOW,
        GREAT,
        ERROR,
    };

    // 算符优先文法
    class OPG {
        template<typename T>
        using Matrix = std::vector<std::vector<T>>;

        // IO
        friend std::istream& operator>>(std::istream& in, OPG &rhs) {
            Symbol ch;

            in >> rhs._numVn >> rhs._numVt >> rhs._numRule;

            // 读入非终结符
            for (int i = 0; i < rhs._numVn; ++i) {
                in >> ch;
                rhs._Vn.insert(ch);
            }

            // 读入终结符
            for (int i = 0; i < rhs._numVt; ++i) {
                in >> ch;
                rhs._Vt.insert(ch);
            }

            // 读入文法开始字符
            in >> ch;
            rhs._start = ch;

            std::string s;
            std::getline(in, s);

            // 读入规则
            for (int i = 0; i < rhs._numRule; ++i) {
                Symbol left;
                std::vector<Symbol> right, unified;

                left = in.get();
                in.get(), in.get();
                for (ch = in.get(); ch != '\r' && ch != '\n'; ch = in.get()) {
                    // 构造规则集合
                    right.push_back(ch);
                    // 构造统一后的规则集合
                    unified.push_back(rhs.isVn(ch) ? SYM_VN : ch);
                }

                rhs._rule.insert({ left, right });
                // 去除 N ::= N
                if (!(unified.size() == 1 && unified.at(0) == SYM_VN)) {
                    rhs._unified.insert(unified);
                }

                do {
                    ch = in.get();
                } while (ch == '\r' || ch == '\n');
                in.unget();
            }

            return in;
        }

    private:
        struct _Hash {
            std::size_t operator() (const vector<Symbol> &v) const {
                return std::hash<char>{}(v[0]) & std::hash<char>{}(v[v.size() - 1]);
            }
        };

        using Unified = std::unordered_set<std::vector<Symbol>, _Hash>;

        // 终结符个数; 非终结符个数; 规则条数
        int _numVt, _numVn, _numRule;
        // 开始字符
        char _start;

        // 终结符集合
        std::unordered_set<Symbol> _Vt;
        // 非终结符集合
        std::unordered_set<Symbol> _Vn;

        // 文法集合
        std::unordered_multimap<Symbol, std::vector<Symbol>> _rule;
        // 统一后的文法集合
        Unified _unified;

        void _insert(std::unordered_map<Symbol, std::unordered_set<Symbol>> &ss,
            std::vector<std::pair<Symbol, Symbol>> &stk, Symbol U, Symbol b) {
            if (auto res = ss[U].find(b); res == ss[U].end()) {
                ss[U].insert(b);
                stk.push_back({ U, b });
            }
        }

        /*
         * 计算FirstVt集合
         * 所有的 b ∈ FirstVt(U) 满足
         * (i)  存在规则 U ::= b... 和 U ::= Vb...
         * (ii) 存在规则 U ::= V... 且 b ∈ FirstVt(V)
         */
        auto _getFirstVt() {
            auto firstVt = std::unordered_map<Symbol, std::unordered_set<Symbol>>();
            auto stk = std::vector<std::pair<Symbol, Symbol>>();

            // 扫描所有 U ::= b... 和 U ::= Vb... 的规则
            for (const auto &[U, right]: _rule) {
                if (isVt(right.front())) {
                    _insert(firstVt, stk, U, right.front());
                } else if (right.size() >= 2
                    && isVn(right.front()) && isVt(right.at(1))) {
                    _insert(firstVt, stk, U, right.at(1));
                }
            }

            // 扫描所有 U ::= V... 中的 b ∈ FirstVt(V)
            while (!stk.empty()) {
                const auto [V, b] = stk.back(); stk.pop_back();
                for (const auto &[U, right]: _rule) {
                    if (right.front() == V) {
                        _insert(firstVt, stk, U, b);
                    }
                }
            }

            return firstVt;
        }

        /*
         * 计算LastVt集合
         * 所有的 a ∈ LastVt(U) 满足
         * (i)  存在规则 U ::= ...a 和 U ::= ...aV
         * (ii) 存在规则 U ::= ...V 且 a ∈ LastVt(V)
         */
        auto _getLastVt() {
            auto lastVt = std::unordered_map<Symbol, std::unordered_set<Symbol>>();
            auto stk = std::vector<std::pair<Symbol, Symbol>>();

            // 扫描所有 U ::= ...a 和 U ::= ...aV 的规则
            for (const auto &[U, right]: _rule) {
                if (isVt(right.back())) {
                    _insert(lastVt, stk, U, right.back());
                } else if (right.size() >= 2
                    && isVn(right.back()) && isVt(right.at(right.size() - 2))) {
                    _insert(lastVt, stk, U, right.at(right.size() - 2));
                }
            }

            // 扫描所有 U ::= ...V 中的 a ∈ LastVt(V)
            while (!stk.empty()) {
                const auto [V, a] = stk.back(); stk.pop_back();
                for (const auto &[U, right]: _rule) {
                    if (right.back() == V) {
                        _insert(lastVt, stk, U, a);
                    }
                }
            }

            return lastVt;
        }

    public:
        bool isVt(const Symbol sym) {
            return _Vt.find(sym) != _Vt.end() || sym == SYM_SEPARATOR;
        }

        bool isVn(const Symbol sym) {
            return _Vn.find(sym) != _Vn.end() || sym == SYM_VN;
        }

        /*
         * 计算优先关系矩阵
         * (i)   U ::= ...ab... => a = b
         * (ii)  U ::= ...aVb... => a = b
         * (iii) U ::= ...aW... 且 b ∈ FirstVt(W) => a < b
         * (iv)  U ::= ...Wb... 且 a ∈ LastVt(W) => a > b
         */
        auto getPrecedence() {
            auto precedence = Matrix<Precedence>(
                129, std::vector<Precedence>(129, Precedence::ERROR)
            );
            auto firstVt = _getFirstVt();
            auto lastVt = _getLastVt();

            for (const auto &[U, right]: _rule) {
                for (int i = 0, n = right.size(); i < n - 1; ++i) {
                    auto x0 = right.at(i), x1 = right.at(i + 1);

                    // U ::= ...ab... => a = b
                    if (isVt(x0) && isVt(x1)) {
                        precedence[x0][x1] = Precedence::EQUAL;
                    }

                    // U ::= ...aVb... => a = b
                    if (i < n - 2 && isVt(x0)
                        && isVt(right.at(i + 2)) && isVn(x1)) {
                        precedence[x0][right.at(i + 2)] = Precedence::EQUAL;
                    }

                    // U ::= ...aW... 且 b ∈ FirstVt(W) => a < b
                    if (isVt(x0) && isVn(x1)) {
                        for (const auto b: firstVt[x1]) {
                            precedence[x0][b] = Precedence::BELOW;
                        }
                    }

                    // U ::= ...Wb... 且 a ∈ LastVt(W) => a > b
                    if (isVn(x0) && isVt(x1)) {
                        for (const auto a: lastVt[x0]) {
                            precedence[a][x1] = Precedence::GREAT;
                        }
                    }
                }
            }

            // 加入'#'的优先级
            for (const auto &Vt: _Vt) {
                precedence[Vt]['#'] = Precedence::GREAT;
                precedence['#'][Vt] = Precedence::BELOW;
            }

            return precedence;
        }

        Unified getUnifiedRules() {
            return _unified;
        }
    };

    // OPG分析器
    class Parser {
    private:
        // OPG文法
        OPG _G;
        // 每一个句子都是一个输入字符流
        std::stringstream _in;
        Symbol _symbol;

        // 读取下一个Symbol
        void _next() {
            int ch;

            if (ch = _in.get(); _G.isVn(ch)) {
                _symbol = SYM_VN;
            } else if (_G.isVt(ch)) {
                _symbol = ch;
            } else if (ch == '#') {
                _symbol = SYM_SEPARATOR;
            } else {
                _symbol = SYM_ERROR;
            }
        }

    public:
        Parser(OPG &g): _G(g) { }
        ~Parser() = default;

        // 判断句子 s 是否符合 _G 文法
        bool isValiad(const std::string &s) {
            _in.str("");
            _in.clear();
            _in << s << SYM_SEPARATOR;

            // 符号栈
            auto stk = std::vector<Symbol>();
            // 栈顶终结符
            Symbol topVt;

            stk.push_back(SYM_SEPARATOR);
            topVt = SYM_SEPARATOR;

            auto p = _G.getPrecedence();

            _next();
            while (true) {
                switch (p[topVt][_symbol]) {
                    // top <= sym => 移进
                    case Precedence::EQUAL:
                    case Precedence::BELOW: {
                        stk.push_back(_symbol);
                        topVt = _symbol;
                        _next();
                        break;
                    }
                    // top > sym => 规约
                    case Precedence::GREAT: {
                        auto flag = false;

                        // 使用统一后的文法进行规约
                        for (const auto &rule: _G.getUnifiedRules()) {
                            auto len = rule.size();

                            if (stk.size() < len + 1) {
                                continue;
                            }

                            // 将栈顶与每一条文法进行匹配 匹配成功进行规约
                            auto tmp = std::vector<Symbol>(stk.end() - len, stk.end());
                            if (tmp == rule) {
                                while (len--) {
                                    stk.pop_back();
                                }
                                stk.push_back(SYM_VN);
                                flag = true;
                                break;
                            }
                        }

                        // 规约完成后重新定位topVt
                        // 找不到文法进行规约 或者 符号栈里的符号小于两个
                        if (!flag || stk.size() < 2) {
                            return false;
                        }

                        // 输入# 且栈内是 [#, Vn] 结束循环
                        if (_symbol == SYM_SEPARATOR && stk.size() == 2
                            && _G.isVn(stk.back())) {
                            return true;
                        }

                        // 规约之后栈顶一定是非终结符Vn
                        // 即[#, ..., Vt, Vn]
                        if (auto vt = stk.at(stk.size() - 2); _G.isVt(vt)) {
                            topVt = vt;
                            continue;
                        }
                        // 栈顶连续两个Vn报错
                        // 即[#, ..., Vn, Vn]
                        return false;
                    }
                    case Precedence::ERROR: {
                        return false;
                    }
                }
            }

            return false;
        }
    };
}

#endif

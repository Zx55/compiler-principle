/*
 * Automata.h
 * Implemention of Automata algorithm.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef __COMPILER_AUTOMATA_
#define __COMPILER_AUTOMATA_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace Automata {
    class DFA {
        using _ll = long long;
        friend class NFA;

        // IO
        friend std::istream& operator>>(std::istream& in, DFA &rhs) {
            in >> rhs._stateNum >> rhs._endNum >> rhs._transNum;

            // 孤岛状态"":
            // 表示该状态连接到一个不存在的状态，即该状态是不可接受状态
            rhs._automata[""] = _State();
            for (int i = 0; i < rhs._stateNum; ++i) {
                std::string name;
                in >> name;

                rhs._automata[name] = _State();
            }

            in >> rhs._start;

            for (int i = 0; i < rhs._endNum; ++i) {
                std::string name;
                in >> name;

                rhs._automata[name]._isEnd = true;
            }

            for (int i = 0; i < rhs._transNum; ++i) {
                std::string state1, input, state2;
                in >> state1 >> input >> state2;

                rhs._automata[state1]._transform[input[1]] = state2;
                rhs._charSet.insert(input[1]);
            }

            return in;
        }

        friend std::ostream& operator<<(std::ostream& out, DFA &rhs) {
            out << rhs._stateNum << " " << rhs._endNum
                << " " << rhs._transNum << std::endl;

            // 因为OJ的对于空格敏感 即对输入一个空格会判错
            // 又C/C++输出退格'\b'再输入换行'\n'会出错
            // 所以采用了stringstream的方式曲线救国 orz
            std::stringstream ss;

            // 这里state是一个pair<string, _State>
            // first是状态名 second是对应的状态State数据结构
            for (const auto &state: rhs) {
                const auto &[ sname, sstate ] = state;

                if (sname == "") {
                    continue;
                }
                ss << sname << " ";
            }
            out << ss.str().substr(0, ss.str().size() - 1) << std::endl;

            out << rhs._start << std::endl;

            ss.str("");
            ss.clear();
            for (const auto &state: rhs) {
                if (const auto &[ sname, sstate ] = state; sstate._isEnd) {
                    ss << sname << " ";
                }
            }
            out << ss.str().substr(0, ss.str().size() - 1) << std::endl;

            for (const auto &state: rhs) {
                const auto &[ sname, sstate ] = state;

                if (sname == "") {
                    continue;
                }

                for (const auto ch: rhs._charSet) {
                    if (sstate._transform[ch] != "") {
                        out << sname << " \"" << ch << "\" "
                            << sstate._transform[ch] << std::endl;
                    }
                }
            }

            return out;
        }

    private:
        // DFA单值状态数据结构 包含单值转移函数
        struct _State {
            // 转移矩阵
            std::vector<std::string> _transform;
            // 该状态是否是终态
            bool _isEnd;

            _State(bool isEnd = false):
                _transform(std::vector<std::string>(127, "")), _isEnd(isEnd) { }
            ~_State() { }
        };

        // 在判定等价时用到的 表示两个状态是否访问过的数据结构
        class _Visit {
        private:
            // 为std::pair定义hash
            struct _hash {
                template<typename T1, typename T2>
                std::size_t operator() (const std::pair<T1, T2>& p) const {
                    auto h1 = std::hash<T1>{}(p.first);
                    auto h2 = std::hash<T2>{}(p.second);
                    return h1 ^ h2;
                }
            };

            std::unordered_map<std::pair<std::string, std::string>, bool, _hash> _visit;

        public:
            _Visit() = default;
            ~_Visit() { }

            // getter和setter
            bool &operator()(const std::string &s1, const std::string &s2) {
                return _visit[make_pair(s1, s2)];
            }
        };

        // 最小化时用到的状态集数据结构
        struct _StateSet {
            std::set<std::string> _set;
            // 该集合中的所有状态是否转移到相同的状态集
            bool _flag;

            _StateSet(bool flag = false): _flag(flag) { }
            ~_StateSet() { }

            // 封装了_set的一些函数
            auto begin() {
                return _set.begin();
            }

            auto end() {
                return _set.end();
            }

            auto size() {
                return _set.size();
            }

            auto insert(std::string s) {
                return _set.insert(s);
            }

            auto find(std::string s) {
                return _set.find(s);
            }
        };

        // 状态机: 从 状态名 → 单值状态 的映射
        std::unordered_map<std::string, _State> _automata;
        // 字符集
        std::unordered_set<char> _charSet;
        // 开始状态
        std::string _start;
        // 总状态数
        _ll _stateNum;
        // 总终态数
        _ll _endNum;
        // 总转移函数数
        _ll _transNum;

        bool _isEnd(const std::string &s1, const std::string &s2, DFA &rhs) {
            // 当且仅当一状态是终态 另一状态非终态 => 两个DFA不等价
            return _automata[s1]._isEnd ^ rhs._automata[s2]._isEnd;
        }

        bool _dfsEqual(const std::string &s1, const std::string &s2,
            DFA &rhs, _Visit &vis) {
            if (_isEnd(s1, s2, rhs)) {
                return true;
            }

            vis(s1, s2) = true;

            for (const auto ch: _charSet) {
                auto next1 = _automata[s1]._transform[ch];
                auto next2 = rhs._automata[s2]._transform[ch];
                if (!vis(next1, next2) && _dfsEqual(next1, next2, rhs, vis)) {
                    return true;
                }
            }

            return false;
        }

        void _dfsRedundancy(const std::string &start, _StateSet &vis) {
            vis.insert(start);

            for (auto const ch: _charSet) {
                auto next = _automata[start]._transform[ch];

                if (next == "") {
                    continue;
                }

                if (vis.find(next) == vis.end()) {
                    _dfsRedundancy(next, vis);
                }
            }
        }

        // 最小化时为了给状态集中的每一种转移状态分组 计算每一种情况的hash
        // 公式是 hash = pos1 & transState1 ^ pos2 & transState2 ^ ...
        size_t _hash(const std::string &state, std::unordered_map<std::string, int> &map) {
            auto transform = _automata[state]._transform;
            size_t res = 0;
            int index = 0;

            for (const auto ch: _charSet) {
                // 转移到的状态 在重新映射后的分组号
                auto groupId = map[transform[ch]];
                auto thisHash = std::hash<std::string>{}("trans" + std::to_string(index++))
                    & std::hash<std::string>{}(std::to_string(groupId));
                res = (res == 0) ? thisHash : res ^ thisHash;
            }

            return res;
        }

        // 根据_State中的_transform重新计算DFA的状态数、终态数和转移数
        void _restoreFromTransform() {
            _stateNum = _automata.size() - 1;
            _endNum = 0;
            _transNum = 0;

            for (const auto &state: _automata) {
                const auto &[ sname, sstate ] = state;

                if (sstate._isEnd) {
                    ++_endNum;
                }

                for (const auto ch: _charSet) {
                    auto next = _automata[sname]._transform[ch];
                    if (next == "") {
                        continue;
                    }

                    ++_transNum;
                }
            }
        }

    public:
        DFA() = default;
        ~DFA() { }

        // 封装_automata的迭代器
        std::unordered_map<std::string, _State>::iterator begin() {
            return _automata.begin();
        }

        std::unordered_map<std::string, _State>::iterator end() {
            return _automata.end();
        }

        /*
         * 两个DFA的等价性判断
         * 参考了 https://www.cnblogs.com/lfri/p/11425266.html 的原理以及实现
         * 简单来说就是 一个DFA取补集 若该补集和另一个DFA不相交 <=> 等价
         */
        bool operator==(DFA rhs) {
            auto vis = _Visit();

            // 初始化vis
            for (const auto &state1: _automata) {
                for (const auto &state2: rhs) {
                    vis(state1.first, state2.first) = false;
                    vis("", state2.first) = false;
                }
                vis(state1.first, "") = false;
            }
            vis("", "") = false;

            return !_dfsEqual(_start, rhs._start, rhs, vis);
        }

        // 去除不可达状态
        void removeRedundancy () {
            auto vis = _StateSet(), visComp = _StateSet();
            _dfsRedundancy(_start, vis);

            // 这里先遍历选出要删除的key 再通过key删除
            // 虽然通过迭代器一边遍历一边删除也可以 但是比较麻烦 而且也写出了点问题 orz
            for (const auto &state: _automata) {
                const auto &[ sname, sstate ] = state;

                if (sname == "") {
                    continue;
                }

                if (vis.find(sname) == vis.end()) {
                    visComp.insert(sname);
                }
            }

            for (const auto &state: visComp) {
                _automata.erase(state);
            }

            _restoreFromTransform();
        }

        /* DFA最小化: 模拟分割法
         * 1. 将状态集分为状态集和非终态集 并放入一个队列q
         * 2. 不断循环 根据集合中每个状态的不同转移状态对q中的集合进行分割
         * 2. 分割后的集合若只有一个元素 => 移出队列
         *    否则将分割后的状态集放回队列 并重新编号
         * 3. 满足下面其中一个条件算跳出循环
         *    (i)  队列为空
         *    (ii) 队列中的每一个状态集都满足: 其中的所有状态的转移状态都是相同的
         * 4. 根据重新映射构建最小的DFA
         */
        DFA minimize() {
            // 先去除不可达状态
            removeRedundancy();

            // 队列q
            auto q = std::deque<_StateSet>();

            // 映射map: 原先的状态名 → 重新分组后的编号
            auto state2Group = std::unordered_map<std::string, int>();
            // 最大组号
            auto maxGroup = 1;
            // 孤岛状态即不可接受状态 编号0
            state2Group[""] = 0;

            // 根据终点和非终点划分
            auto ends = _StateSet();
            auto endsComp = _StateSet();
            for (const auto &state: _automata) {
                const auto &[ sname, sstate ] = state;

                if (sname == "") {
                    continue;
                }

                state2Group[sname] = sstate._isEnd ? 1 : 2;
                if (sstate._isEnd) {
                    ends.insert(sname);
                } else {
                    endsComp.insert(sname);
                    maxGroup = 2;
                }
            }
            if (q.push_back(ends); maxGroup == 2) {
                q.push_back(endsComp);
            }

            while (!q.empty()) {
                auto s = q.front(); q.pop_front();

                // 移出只有一个状态的集合
                if (s.size() == 1) {
                    continue;
                }

                // 根据集合中的每个状态的转移进行分组
                // 方法是计算每种状态的hash
                auto hashGroup = std::unordered_map<std::size_t, _StateSet>();
                for (const auto &state: s) {
                    auto thisHash = _hash(state, state2Group);
                    hashGroup[thisHash].insert(state);
                }

                // 若集合中的所有状态只有一种转移状态 将flag进行置位
                if (hashGroup.size() == 1) {
                    hashGroup.begin()->second._flag = true;
                }

                // 1. 将分割后的集合放回队列q
                // 2. 根据分组情况对集合中的每个状态进行重新编号
                //    第一组编号不变 之后的组根据maxGroup编号
                auto it = hashGroup.begin();
                for (q.push_back(it->second), ++it; it != hashGroup.end(); ++it) {
                    ++maxGroup;
                    auto ss = it->second;

                    q.push_back(ss);
                    for (const auto &state: ss) {
                        state2Group[state] = maxGroup;
                    }
                }

                // 当满足下面的条件时跳出循环:
                // 队列中的每一个状态集都满足: 其中的所有状态的转移状态都是相同的
                bool flag = true;
                for (const auto &ss: q) {
                    flag &= ss._flag;
                }
                if (flag) {
                    goto done;
                }
            }

        done:
            // 根据重新分组后的映射state2Group构造DFA
            auto res = DFA();

            for (const auto &group: state2Group) {
                const auto &[gname, gid] = group;

                if (gname == "") {
                    continue;
                }

                // 为了从"s0"开始命名 因此要-1
                auto newState = "s" + std::to_string(gid - 1);
                res._automata[newState]._isEnd = _automata[gname]._isEnd;
                for (const auto ch: _charSet) {
                    auto next = _automata[gname]._transform[ch];
                    if (next == "") {
                        continue;
                    }

                    res._automata[newState]._transform[ch]
                        = "s" + std::to_string(state2Group[next] - 1);
                }
            }
            // 加入孤岛状态
            res._automata[""] = _State();

            // 设置DFA的其他属性
            res._start = "s" + std::to_string(state2Group[_start] - 1);
            res._charSet = _charSet;
            res._restoreFromTransform();

            return res;
        }
    };

    class NFA {
        using _ll = long long;
        // 用于确定化的闭包数据结构
        using _Closure = std::set<std::string>;

        // IO
        friend std::istream& operator>>(std::istream& in, NFA &rhs) {
            in >> rhs._stateNum >> rhs._endNum >> rhs._transNum;

            // 孤岛状态""
            rhs._automata[""] = _MultiState();
            for (int i = 0; i < rhs._stateNum; ++i) {
                std::string name;
                in >> name;

                rhs._automata[name] = _MultiState();
            }

            in >> rhs._start;

            for (int i = 0; i < rhs._endNum; ++i) {
                std::string name;
                in >> name;

                rhs._automata[name]._isEnd = true;
            }

            for (int i = 0; i < rhs._transNum; ++i) {
                std::string state1, input, state2;

                if (in >> state1 >> input >> state2; input == "\"\"") {
                    rhs._automata[state1]._transform[0].insert(state2);
                } else {
                    rhs._automata[state1]._transform[input[1]].insert(state2);
                    rhs._charSet.insert(input[1]);
                }
            }

            return in;
        }

        friend std::ostream& operator<<(std::ostream& out, NFA &rhs) {
            out << rhs._stateNum << " " << rhs._endNum
                << " " << rhs._transNum << std::endl;

            std::stringstream ss;
            for (const auto &state: rhs) {
                const auto &[sname, sstate] = state;

                if (sname == "") {
                    continue;
                }
                ss << sname << " ";
            }
            out << ss.str().substr(0, ss.str().size() - 1) << std::endl;

            out << rhs._start << std::endl;

            ss.str("");
            ss.clear();
            for (const auto &state: rhs) {
                if (const auto &[sname, sstate] = state; sstate._isEnd) {
                    ss << sname << " ";
                }
            }
            out << ss.str().substr(0, ss.str().size() - 1) << std::endl;

            for (const auto &fromState: rhs) {
                const auto &[fromSname, fromSstate] = fromState;

                if (fromSname == "") {
                    continue;
                }

                auto t = fromSstate._transform[0];
                if (t.size() > 1) {
                    for (const auto &toState: t) {
                        if (toState == "") {
                            continue;
                        }
                        out << fromSname << " \"\" " << toState << std::endl;
                    }
                }

                for (const auto ch: rhs._charSet) {
                    t = fromSstate._transform[ch];
                    if (t.size() > 1) {
                        for (const auto &toState: t) {
                            if (toState == "") {
                                continue;
                            }
                            out << fromSname << " \"" << toState
                                << "\" " << toState << std::endl;
                        }
                    }
                }
            }

            return out;
        }

    private:
        // NFA多值状态数据结构 包含多值转移函数
        struct _MultiState {
            std::vector<std::unordered_set<std::string> > _transform;
            bool _isEnd;

            _MultiState(bool isEnd = false):
                _transform(std::vector<std::unordered_set<std::string> >(127, { "" })),
                _isEnd(isEnd) { }
            ~_MultiState() { }
        };

        // 计算_Closure的hash 为了将_Closure作为哈希表的key
        struct _hash {
            std::size_t operator()(const _Closure &c) const {
                auto it = c.begin();
                std::size_t res = std::hash<std::string>{}(*it);
                for (++it; it != c.end(); ++it) {
                    res ^= std::hash<std::string>{}(*it);
                }
                return res;
            }
        };

        // 状态机: 状态名 → 多值状态
        std::unordered_map<std::string, _MultiState> _automata;
        std::unordered_set<char> _charSet;
        std::string _start;
        _ll _stateNum;
        _ll _endNum;
        _ll _transNum;

        void _dfs(const std::string fromState, _Closure &c,
            std::unordered_set<std::string> &vis) {
            vis.insert(fromState);

            if (auto t = _automata[fromState]._transform[0]; t.size() > 1) {
                for (const auto &toState: t) {
                    if (toState == "") {
                        continue;
                    }

                    c.insert(toState);
                    if (vis.find(toState) == vis.end()) {
                        _dfs(toState, c, vis);
                    }
                }
            }
        }

        // 求ε-闭包: DFS
        _Closure _closure(const _Closure &c) {
            auto vis = std::unordered_set<std::string>();
            auto res = _Closure();

            for (const auto &state: c) {
                res.insert(state);
                if (vis.find(state) == vis.end()) {
                    _dfs(state, res, vis);
                }
            }

            return res;
        }

        // 求闭包c接受字符ch后的ε-闭包
        _Closure _move(const _Closure &c, int ch) {
            auto res = _Closure();

            for (const auto &fromState: c) {
                if (auto next = _automata[fromState]._transform[ch]; next.size() > 1) {
                    for (const auto &toState: next) {
                        if (toState == "") {
                            continue;
                        }

                        res.insert(toState);
                    }
                }
            }

            return _closure(res);
        }

        // 判断闭包c中是否还有终态
        bool _isEnd(const _Closure &c) {
            for (const auto &state: c) {
                if (_automata[state]._isEnd) {
                    return true;
                }
            }

            return false;
        }

    public:
        NFA() = default;
        ~NFA() { }

        // 封装_automata的迭代器
        std::unordered_map<std::string, _MultiState>::iterator begin() {
            return _automata.begin();
        }

        std::unordered_map<std::string, _MultiState>::iterator end() {
            return _automata.end();
        }

        /* NFA确定化: 模拟子集法
         * 1. 将开始状态的ε-闭包加入队列q
         * 2. 不断循环 每次取出队首闭包
         *    计算其接受字符集输入的ε-闭包 若闭包不为空再放入队列q
         * 4. 直到队列为空结束循环
         * 5. 对以上所有的闭包作为状态重新编号输出DFA
         *    其中开始状态的闭包作为DFA的开始状态 带有终态的闭包作为DFA的终态
         *    闭包之间的转移作为DFA的转移
         */
        DFA determine() {
            auto res = DFA();
            res._start = "s0";
            res._automata[""] = DFA::_State(false);

            auto q = std::deque<_Closure>();
            auto vis = std::unordered_set<_Closure, _hash>();

            // 映射map: 闭包 → 重新编号
            auto map = std::unordered_map<_Closure, int, _hash>();
            // 编号count 从0开始
            int index = 0;

            auto start = _Closure({ _start });
            auto startC = _closure(start);
            q.push_back(startC);
            vis.insert(startC);

            // 开始闭包映射为0
            map[startC] = index++;
            res._stateNum++;

            while (!q.empty()) {
                auto c = q.front(); q.pop_front();

                // 带有终态的闭包 作为映射后 DFA的终态
                if (_isEnd(c)) {
                    res._automata["s" + std::to_string(map[c])]._isEnd = true;
                    res._endNum++;
                } else {
                    res._automata["s" + std::to_string(map[c])]._isEnd = false;
                }

                for (const auto ch: _charSet) {
                    if (auto move_c = _move(c, ch); !move_c.empty()) {
                        res._charSet.insert(ch);

                        auto findRes = map.find(move_c);
                        if (findRes == map.end()) {
                            map[move_c] = index++;
                            res._stateNum++;
                        }

                        // 闭包间的转移 作为映射后 DFA状态的转移
                        res._automata["s" + std::to_string(map[c])]._transform[ch]
                            = "s" + std::to_string(map[move_c]);
                        res._transNum++;

                        if (vis.find(move_c) == vis.end()) {
                            vis.insert(move_c);
                            q.push_back(move_c);
                        }
                    }
                }
            }

            return res;
        }

        // 一步到位 先确定化再最小化
        DFA toMinimizedDFA() {
            return determine().minimize();
        }
    };
}

#endif
